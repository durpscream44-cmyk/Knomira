// Copyright (C) 2026 Knomira contributors.
// Resolve a control assignment from RmlUi's hover tree BEFORE its default
// mouse-down/focus behavior. Used in the shell and headless hit-test CI.
#pragma once
#include <RmlUi/Core/Element.h>
#include <string>

namespace KnomiraInput {
inline std::string FindBindingIdAtHover(Rml::Element* hover) {
    constexpr const char* slotPrefix = "binding_slot_";
    constexpr size_t slotPrefixLength = 13; // strlen("binding_slot_")
    for (Rml::Element* element = hover; element; element = element->GetParentNode()) {
        const Rml::String& id = element->GetId();
        if (id.rfind("control_k_", 0) == 0 || id.rfind("control_p_", 0) == 0)
            return id;
        if (id.rfind(slotPrefix, 0) == 0) {
            const std::string actionId = id.substr(slotPrefixLength);
            if (actionId.rfind("control_k_", 0) == 0 || actionId.rfind("control_p_", 0) == 0)
                return actionId;
        }
    }
    return {};
}
} // namespace KnomiraInput
