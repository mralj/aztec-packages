#pragma once

#include <gmock/gmock.h>

#include "barretenberg/vm2/common/memory_types.hpp"
#include "barretenberg/vm2/simulation/alu.hpp"
#include "barretenberg/vm2/simulation/context.hpp"

namespace bb::avm2::simulation {

class MockAlu : public AluInterface {
  public:
    // https://google.github.io/googletest/gmock_cook_book.html#making-the-compilation-faster
    MockAlu();
    ~MockAlu() override;

    MOCK_METHOD(MemoryValue, add, (const MemoryValue& a, const MemoryValue& b), (override));
};

} // namespace bb::avm2::simulation
