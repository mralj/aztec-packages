// === AUDIT STATUS ===
// internal:    { status: not started, auditors: [], date: YYYY-MM-DD }
// external_1:  { status: not started, auditors: [], date: YYYY-MM-DD }
// external_2:  { status: not started, auditors: [], date: YYYY-MM-DD }
// =====================

#pragma once
#include "barretenberg/relations/translator_vm/translator_permutation_relation.hpp"

namespace bb {

/**
 * @brief Compute contribution of the goblin translator permutation relation for a given edge (internal function)
 *
 * @details There are 2 relations associated with enforcing the set permutation relation
 * This file handles the relation that confirms faithful calculation of the grand
 * product polynomial Z_perm.
 *
 *  C(in(X)...) =
 *      ( z_perm(X) + lagrange_first(X) )*P(X)
 *         - ( z_perm_shift(X) + lagrange_last(X))*Q(X),
 * where P(X) = Prod_{i=0:4} numerator_polynomial_i(X) + γ
 *       Q(X) = Prod_{i=0:4} ordered_range_constraint_i(X) + γ
 * the first 4 numerator polynomials are interleaved range constraint polynomials and the last one is the constant
 * extra numerator
 *
 * If operating in zero-knowledge, we mark the positions (via the lagrange_masking polynomial) that
 * contain random values.
 * @param evals transformed to `evals + C(in(X)...)*scaling_factor`
 * @param in an std::array containing the fully extended Univariate edges.
 * @param parameters contains beta, gamma, and public_input_delta, ....
 * @param scaling_factor optional term to scale the evaluation before adding to evals.
 */

template <typename FF>
template <typename ContainerOverSubrelations, typename AllEntities, typename Parameters>
void TranslatorPermutationRelationImpl<FF>::accumulate(ContainerOverSubrelations& accumulators,
                                                       const AllEntities& in,
                                                       const Parameters& params,
                                                       const FF& scaling_factor)
{
    [&]() {
        using Accumulator = std::tuple_element_t<0, ContainerOverSubrelations>;
        using View = typename Accumulator::View;

        const auto z_perm = View(in.z_perm);
        const auto z_perm_shift = View(in.z_perm_shift);
        const auto lagrange_first = View(in.lagrange_first);
        const auto lagrange_last = View(in.lagrange_last);

        // Contribution (1)
        std::get<0>(accumulators) +=
            (((z_perm + lagrange_first) * compute_grand_product_numerator<Accumulator>(in, params)) -
             ((z_perm_shift + lagrange_last) * compute_grand_product_denominator<Accumulator>(in, params))) *
            scaling_factor;
    }();

    [&]() {
        using Accumulator = std::tuple_element_t<1, ContainerOverSubrelations>;
        using View = typename Accumulator::View;

        const auto z_perm_shift = View(in.z_perm_shift);
        const auto lagrange_last = View(in.lagrange_last);

        // Contribution (2)
        std::get<1>(accumulators) += (lagrange_last * z_perm_shift) * scaling_factor;
    }();
};
} // namespace bb
