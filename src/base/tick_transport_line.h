// Copyright (C) 2026 Knomira contributors.
// Lightweight, simulation-tick-based transport line for future conveyors.
// No GUI, Qt, item graphics, pathfinding, or per-item movement each tick.
// This class is NOT hooked up to live inventory or game tech yet.
#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <limits>
#include <stdexcept>
#include <unordered_set>

namespace Knomira {
namespace Logistics {

using Tick = std::uint64_t;
using ItemId = std::uint32_t;

struct TransportConfig {
    std::uint32_t lengthInTiles = 1;
    std::uint32_t ticksPerTile = 4;
    std::uint32_t ticksBetweenItems = 2;
    std::size_t maxItemsInFlight = 64;
    bool nearInstant = false; // Higher-tier option: one-tick travel, still rate-limited.
};

enum class EnqueueResult {
    Accepted,
    Disabled,
    InvalidItem,
    AlreadyInFlight,
    AtCapacity,
    RateLimited,
    SourceUnavailable,
    TickOverflow
};

// Transfer hooks have strict, transaction-like responsibilities:
// - takeFromSource(id): on true, the *authoritative* inventory has put the
//   item in a non-haulable transit state without changing its unique identity.
//   On false it MUST leave the item's state unchanged.
// - deliverToDestination(id): on true, that same item has been placed into
//   available destination capacity and released from transit. On false it
//   MUST leave the item in transit, unchanged.
// - returnToSource(id): on true, the item has been safely restored or moved
//   to an explicitly managed recovery store; on false it remains in transit.
// A real engine adapter must handle crashes, save/load, capacity and source
// reservations in the authoritative inventory. This queue is NOT a ledger.
class TickTransportLine {
public:
    using TransferHook = std::function<bool(ItemId)>;

    explicit TickTransportLine(TransportConfig cfg) : m_config(cfg) {
        if (cfg.lengthInTiles == 0 || cfg.ticksPerTile == 0 ||
            cfg.ticksBetweenItems == 0 || cfg.maxItemsInFlight == 0) {
            throw std::invalid_argument("Conveyor config must use positive dimensions, rates and capacity");
        }
    }

    EnqueueResult Enqueue(ItemId item, Tick tick, const TransferHook& takeFromSource) {
        if (!m_operational) return EnqueueResult::Disabled;
        if (item == 0) return EnqueueResult::InvalidItem;
        if (m_ids.find(item) != m_ids.end()) return EnqueueResult::AlreadyInFlight;
        if (m_items.size() >= m_config.maxItemsInFlight) return EnqueueResult::AtCapacity;
        if (tick < m_nextInputTick) return EnqueueResult::RateLimited;

        const Tick delay = m_config.nearInstant ? Tick{1} :
            Tick{m_config.lengthInTiles} * Tick{m_config.ticksPerTile};
        if (tick > std::numeric_limits<Tick>::max() - delay ||
            tick > std::numeric_limits<Tick>::max() - m_config.ticksBetweenItems) {
            return EnqueueResult::TickOverflow;
        }
        if (!takeFromSource || !takeFromSource(item)) {
            return EnqueueResult::SourceUnavailable;
        }

        // After successful transfer of ownership into transit, these insertions
        // should be made exception-safe in a later engine integration. The
        // prototype assumes allocation success; do not connect to live items
        // until the engine adapter uses a true rollback transaction.
        m_ids.insert(item);
        m_items.push_back({item, tick + delay});
        m_nextInputTick = tick + m_config.ticksBetweenItems;
        return EnqueueResult::Accepted;
    }

    // Called once per simulation tick (or only when NextDueTick has arrived).
    // Does not iterate items before they are due. A destination that rejected
    // a delivery must explicitly signal availability to wake the line again.
    std::size_t Process(Tick tick, const TransferHook& deliverToDestination,
                        std::size_t maxCompletions = 64) {
        if (!m_operational || !m_destinationAvailable || !deliverToDestination)
            return 0;
        std::size_t delivered = 0;
        while (delivered < maxCompletions && !m_items.empty() &&
               m_items.front().dueTick <= tick) {
            const auto id = m_items.front().id;
            if (!deliverToDestination(id)) {
                m_destinationAvailable = false;
                break;
            }
            m_items.pop_front();
            m_ids.erase(id);
            ++delivered;
        }
        return delivered;
    }

    // Safely drain a disabled/destroyed line into an authoritative recovery
    // store. Failed recoveries remain in transit and may be retried.
    std::size_t Evacuate(const TransferHook& returnToSource) {
        if (!returnToSource) return 0;
        std::size_t moved = 0;
        for (auto it = m_items.begin(); it != m_items.end();) {
            if (returnToSource(it->id)) {
                m_ids.erase(it->id);
                it = m_items.erase(it);
                ++moved;
            } else {
                ++it;
            }
        }
        return moved;
    }

    void SetOperational(bool operational) { m_operational = operational; }
    void SetDestinationAvailable(bool available) { m_destinationAvailable = available; }

    bool Operational() const { return m_operational; }
    bool DestinationAvailable() const { return m_destinationAvailable; }
    std::size_t InFlight() const { return m_items.size(); }
    bool Contains(ItemId item) const { return m_ids.find(item) != m_ids.end(); }
    Tick NextDueTick() const {
        return m_items.empty() ? std::numeric_limits<Tick>::max() : m_items.front().dueTick;
    }
    Tick NextInputTick() const { return m_nextInputTick; }

private:
    struct Packet { ItemId id; Tick dueTick; };
    TransportConfig m_config;
    std::deque<Packet> m_items;
    std::unordered_set<ItemId> m_ids;
    Tick m_nextInputTick = 0;
    bool m_operational = false; // Explicit unlock/connection required.
    bool m_destinationAvailable = true;
};

} // namespace Logistics
} // namespace Knomira
