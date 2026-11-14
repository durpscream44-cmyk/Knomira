// Copyright (C) 2026 Knomira contributors.
// Standalone tests for future tick-based conveyor transfers.
// Runs without Qt, the game engine, Noesis or a display.
#include "../../src/base/tick_transport_line.h"

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

using namespace Knomira::Logistics;

enum class Location { Source, Transit, Destination, Recovered };
struct ItemLedger {
    std::unordered_map<ItemId, Location> items;
    bool destinationHasRoom = true;
    std::size_t movedOut = 0;
    std::size_t movedIn = 0;
    std::size_t deliveryCalls = 0;

    bool Take(ItemId id) {
        auto it = items.find(id);
        if (it == items.end() || it->second != Location::Source) return false;
        it->second = Location::Transit;
        ++movedOut;
        return true;
    }
    bool Deliver(ItemId id) {
        ++deliveryCalls;
        auto it = items.find(id);
        if (!destinationHasRoom || it == items.end() ||
            it->second != Location::Transit) return false;
        it->second = Location::Destination;
        ++movedIn;
        return true;
    }
    bool Recover(ItemId id) {
        auto it = items.find(id);
        if (it == items.end() || it->second != Location::Transit) return false;
        it->second = Location::Recovered;
        return true;
    }
};

int main() {
    // Transport starts locked; ordinary belt takes 12 ticks for 3 tiles,
    // with a 2-tick minimum spacing between input items.
    TickTransportLine line({3, 4, 2, 4, false});
    ItemLedger ledger;
    ledger.items = {{101, Location::Source}, {102, Location::Source},
                    {103, Location::Source}, {104, Location::Source}};
    auto take = [&](ItemId id) { return ledger.Take(id); };
    auto deliver = [&](ItemId id) { return ledger.Deliver(id); };

    assert(line.Enqueue(101, 0, take) == EnqueueResult::Disabled);
    assert(ledger.items.at(101) == Location::Source);
    line.SetOperational(true);
    assert(line.Enqueue(0, 0, take) == EnqueueResult::InvalidItem);
    assert(line.Enqueue(999, 0, take) == EnqueueResult::SourceUnavailable);
    assert(line.InFlight() == 0);
    assert(line.Enqueue(101, 0, take) == EnqueueResult::Accepted);
    assert(line.Enqueue(101, 2, take) == EnqueueResult::AlreadyInFlight);
    assert(line.Enqueue(102, 0, take) == EnqueueResult::RateLimited);
    assert(line.Enqueue(102, 2, take) == EnqueueResult::Accepted);
    assert(line.Enqueue(103, 4, take) == EnqueueResult::Accepted);
    assert(line.Enqueue(104, 6, take) == EnqueueResult::Accepted);
    assert(line.Enqueue(104, 8, take) == EnqueueResult::AlreadyInFlight);
    assert(line.Enqueue(999, 8, take) == EnqueueResult::AtCapacity);
    assert(line.InFlight() == 4);
    assert(line.NextDueTick() == 12);
    for (Tick t = 0; t < 12; ++t) {
        assert(line.Process(t, deliver) == 0);
    }
    assert(ledger.deliveryCalls == 0);

    // Destination full: do not retry for every simulation tick; wait for
    // explicit destination-capacity event, preserving item in transit.
    ledger.destinationHasRoom = false;
    assert(line.Process(12, deliver) == 0);
    assert(!line.DestinationAvailable());
    assert(ledger.deliveryCalls == 1);
    for (Tick t = 13; t <= 100; ++t) {
        assert(line.Process(t, deliver) == 0);
    }
    assert(ledger.deliveryCalls == 1);
    assert(ledger.items.at(101) == Location::Transit);
    ledger.destinationHasRoom = true;
    line.SetDestinationAvailable(true);
    assert(line.Process(101, deliver, 2) == 2); // bounded throughput per step
    assert(line.Process(101, deliver, 2) == 2);
    assert(line.InFlight() == 0);
    assert(ledger.movedOut == 4 && ledger.movedIn == 4);
    assert(line.Process(200, deliver) == 0);
    assert(ledger.movedIn == 4); // no duplicate delivery

    // Route removal: evacuate outstanding items only after recovery succeeds.
    TickTransportLine toRemove({2, 3, 1, 8, false});
    toRemove.SetOperational(true);
    ledger.items[201] = Location::Source;
    ledger.items[202] = Location::Source;
    assert(toRemove.Enqueue(201, 0, take) == EnqueueResult::Accepted);
    assert(toRemove.Enqueue(202, 1, take) == EnqueueResult::Accepted);
    toRemove.SetOperational(false);
    assert(toRemove.Process(500, deliver) == 0);
    assert(toRemove.Evacuate([&](ItemId id) {
        return id != 201 && ledger.Recover(id);
    }) == 1);
    assert(toRemove.InFlight() == 1 && toRemove.Contains(201));
    assert(toRemove.Evacuate([&](ItemId id) { return ledger.Recover(id); }) == 1);
    assert(toRemove.InFlight() == 0);
    assert(ledger.items.at(201) == Location::Recovered);
    assert(ledger.items.at(202) == Location::Recovered);

    // A later technology could offer virtually instantaneous *travel*, but
    // rate and capacity still limit the transfer. It is not unlimited teleport.
    TickTransportLine fast({100, 50, 3, 2, true});
    fast.SetOperational(true);
    ledger.items[301] = Location::Source;
    ledger.items[302] = Location::Source;
    assert(fast.Enqueue(301, 40, take) == EnqueueResult::Accepted);
    assert(fast.Enqueue(302, 42, take) == EnqueueResult::RateLimited);
    assert(fast.Process(40, deliver) == 0);
    assert(fast.Process(41, deliver) == 1);
    assert(fast.Enqueue(302, 43, take) == EnqueueResult::Accepted);
    assert(fast.Process(44, deliver) == 1);

    // The transport core must keep stable IDs; no item is instantiated twice.
    std::unordered_set<ItemId> seen;
    for (const auto& kv : ledger.items) {
        assert(seen.insert(kv.first).second);
        assert(kv.second != Location::Transit);
    }

    // 20,000 transfers in chronological order. No movement per belt tile;
    // each event is accepted and delivered once when due. Do not benchmark
    // speed here because CI runner timing is not stable.
    TickTransportLine stress({50, 2, 1, 2048, false});
    stress.SetOperational(true);
    ItemLedger many;
    std::size_t totalDelivered = 0;
    for (std::uint32_t i = 1; i <= 20000; ++i) {
        many.items.emplace(i, Location::Source);
        assert(stress.Enqueue(i, i, [&](ItemId id) { return many.Take(id); }) ==
               EnqueueResult::Accepted);
        totalDelivered += stress.Process(i, [&](ItemId id) { return many.Deliver(id); });
    }
    totalDelivered += stress.Process(20101, [&](ItemId id) { return many.Deliver(id); }, 30000);
    assert(totalDelivered == 20000);
    assert(stress.InFlight() == 0);
    assert(many.movedOut == 20000 && many.movedIn == 20000);

    // Refuse invalid timing/configuration and overflow rather than wrap.
    bool invalidThrown = false;
    try { TickTransportLine invalid({1, 0, 1, 1, false}); }
    catch (const std::invalid_argument&) { invalidThrown = true; }
    assert(invalidThrown);
    TickTransportLine overflow({1, 2, 2, 2, false});
    overflow.SetOperational(true);
    ledger.items[401] = Location::Source;
    assert(overflow.Enqueue(401, std::numeric_limits<Tick>::max(), take) ==
           EnqueueResult::TickOverflow);
    assert(ledger.items.at(401) == Location::Source);

    std::puts("Tick transport line tests passed (rate, delay, blocking, recovery, integrity, stress).");
    return 0;
}
