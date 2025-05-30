#pragma once
#include "barretenberg/circuit_checker/ultra_circuit_checker.hpp"
#include "barretenberg/stdlib_circuit_builders/ultra_circuit_builder.hpp"

// TODO(https://github.com/AztecProtocol/barretenberg/issues/928): Reorganize

namespace bb {
template <typename T>
concept IsCheckable = bb::IsAnyOf<T, UltraCircuitBuilder, MegaCircuitBuilder>;

/**
 * @brief The unified interface for check circuit functionality implemented in the specialized CircuitChecker classes
 *
 */
class CircuitChecker {
  public:
    template <typename Builder> static bool check(const Builder& builder)
    {
        static_assert(IsCheckable<Builder>);
        if constexpr (IsUltraBuilder<Builder> || IsMegaBuilder<Builder>) {
            return UltraCircuitChecker::check(builder);
        } else {
            return false;
        }
    }
};

} // namespace bb
