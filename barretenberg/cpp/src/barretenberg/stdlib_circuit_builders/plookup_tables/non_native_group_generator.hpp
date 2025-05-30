// === AUDIT STATUS ===
// internal:    { status: not started, auditors: [], date: YYYY-MM-DD }
// external_1:  { status: not started, auditors: [], date: YYYY-MM-DD }
// external_2:  { status: not started, auditors: [], date: YYYY-MM-DD }
// =====================

#pragma once

#include "./types.hpp"
#include "barretenberg/ecc/curves/bn254/fr.hpp"
#include "barretenberg/ecc/curves/bn254/g1.hpp"
#include "barretenberg/ecc/curves/secp256k1/secp256k1.hpp"
#include <array>

namespace bb::plookup::ecc_generator_tables {

template <typename G1> class ecc_generator_table {
  public:
    using element = typename G1::element;
    /**
     * Store arrays of precomputed 8-bit lookup tables for generator point coordinates (and their endomorphism
     *equivalents)
     **/
    inline static std::array<std::pair<fr, fr>, 256> generator_endo_xlo_table;
    inline static std::array<std::pair<fr, fr>, 256> generator_endo_xhi_table;
    inline static std::array<std::pair<fr, fr>, 256> generator_xlo_table;
    inline static std::array<std::pair<fr, fr>, 256> generator_xhi_table;
    inline static std::array<std::pair<fr, fr>, 256> generator_ylo_table;
    inline static std::array<std::pair<fr, fr>, 256> generator_yhi_table;
    inline static std::array<std::pair<fr, fr>, 256> generator_xyprime_table;
    inline static std::array<std::pair<fr, fr>, 256> generator_endo_xyprime_table;
    inline static bool init = false;

    static void init_generator_tables();

    static size_t convert_position_to_shifted_naf(const size_t position);
    static size_t convert_shifted_naf_to_position(const size_t shifted_naf);
    static std::array<fr, 2> get_xlo_endo_values(const std::array<uint64_t, 2> key);
    static std::array<fr, 2> get_xhi_endo_values(const std::array<uint64_t, 2> key);
    static std::array<fr, 2> get_xlo_values(const std::array<uint64_t, 2> key);
    static std::array<fr, 2> get_xhi_values(const std::array<uint64_t, 2> key);
    static std::array<fr, 2> get_ylo_values(const std::array<uint64_t, 2> key);
    static std::array<fr, 2> get_yhi_values(const std::array<uint64_t, 2> key);
    static std::array<fr, 2> get_xyprime_values(const std::array<uint64_t, 2> key);
    static std::array<fr, 2> get_xyprime_endo_values(const std::array<uint64_t, 2> key);
    static BasicTable generate_xlo_table(BasicTableId id, const size_t table_index);
    static BasicTable generate_xhi_table(BasicTableId id, const size_t table_index);
    static BasicTable generate_xlo_endo_table(BasicTableId id, const size_t table_index);
    static BasicTable generate_xhi_endo_table(BasicTableId id, const size_t table_index);
    static BasicTable generate_ylo_table(BasicTableId id, const size_t table_index);
    static BasicTable generate_yhi_table(BasicTableId id, const size_t table_index);
    static BasicTable generate_xyprime_table(BasicTableId id, const size_t table_index);
    static BasicTable generate_xyprime_endo_table(BasicTableId id, const size_t table_index);
    static MultiTable get_xlo_table(const MultiTableId id, const BasicTableId basic_id);
    static MultiTable get_xhi_table(const MultiTableId id, const BasicTableId basic_id);
    static MultiTable get_xlo_endo_table(const MultiTableId id, const BasicTableId basic_id);
    static MultiTable get_xhi_endo_table(const MultiTableId id, const BasicTableId basic_id);
    static MultiTable get_ylo_table(const MultiTableId id, const BasicTableId basic_id);
    static MultiTable get_yhi_table(const MultiTableId id, const BasicTableId basic_id);
    static MultiTable get_xyprime_table(const MultiTableId id, const BasicTableId basic_id);
    static MultiTable get_xyprime_endo_table(const MultiTableId id, const BasicTableId basic_id);
};

} // namespace bb::plookup::ecc_generator_tables
