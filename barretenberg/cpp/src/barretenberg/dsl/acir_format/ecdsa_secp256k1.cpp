// === AUDIT STATUS ===
// internal:    { status: not started, auditors: [], date: YYYY-MM-DD }
// external_1:  { status: not started, auditors: [], date: YYYY-MM-DD }
// external_2:  { status: not started, auditors: [], date: YYYY-MM-DD }
// =====================

#include "ecdsa_secp256k1.hpp"
#include "barretenberg/stdlib/encryption/ecdsa/ecdsa.hpp"

namespace acir_format {

using namespace bb;
using secp256k1_ct = bb::stdlib::secp256k1<Builder>;

template <typename Builder>
secp256k1_ct::g1_ct ecdsa_convert_inputs(Builder* ctx, const bb::secp256k1::g1::affine_element& input)
{
    uint256_t x_u256(input.x);
    uint256_t y_u256(input.y);
    secp256k1_ct::fq_ct x(
        witness_ct(ctx, bb::fr(x_u256.slice(0, secp256k1_ct::fq_ct::NUM_LIMB_BITS * 2))),
        witness_ct(
            ctx, bb::fr(x_u256.slice(secp256k1_ct::fq_ct::NUM_LIMB_BITS * 2, secp256k1_ct::fq_ct::NUM_LIMB_BITS * 4))));
    secp256k1_ct::fq_ct y(
        witness_ct(ctx, bb::fr(y_u256.slice(0, secp256k1_ct::fq_ct::NUM_LIMB_BITS * 2))),
        witness_ct(
            ctx, bb::fr(y_u256.slice(secp256k1_ct::fq_ct::NUM_LIMB_BITS * 2, secp256k1_ct::fq_ct::NUM_LIMB_BITS * 4))));

    return { x, y };
}

witness_ct ecdsa_index_to_witness(Builder& builder, uint32_t index)
{
    fr value = builder.get_variable(index);
    return { &builder, value };
}

template <typename Builder>
void create_ecdsa_k1_verify_constraints(Builder& builder,
                                        const EcdsaSecp256k1Constraint& input,
                                        bool has_valid_witness_assignments)
{
    using secp256k1_ct = bb::stdlib::secp256k1<Builder>;
    using field_ct = bb::stdlib::field_t<Builder>;
    using bool_ct = bb::stdlib::bool_t<Builder>;
    using byte_array_ct = bb::stdlib::byte_array<Builder>;

    if (has_valid_witness_assignments == false) {
        dummy_ecdsa_constraint(builder, input);
    }

    auto new_sig = ecdsa_convert_signature(builder, input.signature);

    byte_array_ct message = ecdsa_array_of_bytes_to_byte_array(builder, input.hashed_message);
    auto pub_key_x_byte_arr = ecdsa_array_of_bytes_to_byte_array(builder, input.pub_x_indices);
    auto pub_key_y_byte_arr = ecdsa_array_of_bytes_to_byte_array(builder, input.pub_y_indices);

    auto pub_key_x_fq = typename secp256k1_ct::fq_ct(pub_key_x_byte_arr);
    auto pub_key_y_fq = typename secp256k1_ct::fq_ct(pub_key_y_byte_arr);

    std::vector<uint8_t> rr(new_sig.r.begin(), new_sig.r.end());
    std::vector<uint8_t> ss(new_sig.s.begin(), new_sig.s.end());
    uint8_t vv = new_sig.v;

    stdlib::ecdsa_signature<Builder> sig{ stdlib::byte_array<Builder>(&builder, rr),
                                          stdlib::byte_array<Builder>(&builder, ss),
                                          stdlib::uint8<Builder>(&builder, vv) };

    pub_key_x_fq.assert_is_in_field();
    pub_key_y_fq.assert_is_in_field();
    typename secp256k1_ct::g1_bigfr_ct public_key = typename secp256k1_ct::g1_bigfr_ct(pub_key_x_fq, pub_key_y_fq);
    for (size_t i = 0; i < 32; ++i) {
        sig.r[i].assert_equal(field_ct::from_witness_index(&builder, input.signature[i]));
        sig.s[i].assert_equal(field_ct::from_witness_index(&builder, input.signature[i + 32]));
        pub_key_x_byte_arr[i].assert_equal(field_ct::from_witness_index(&builder, input.pub_x_indices[i]));
        pub_key_y_byte_arr[i].assert_equal(field_ct::from_witness_index(&builder, input.pub_y_indices[i]));
    }
    for (size_t i = 0; i < input.hashed_message.size(); ++i) {
        message[i].assert_equal(field_ct::from_witness_index(&builder, input.hashed_message[i]));
    }

    bool_ct signature_result =
        stdlib::ecdsa_verify_signature_prehashed_message_noassert<Builder,
                                                                  secp256k1_ct,
                                                                  typename secp256k1_ct::fq_ct,
                                                                  typename secp256k1_ct::bigfr_ct,
                                                                  typename secp256k1_ct::g1_bigfr_ct>(
            message, public_key, sig);
    bool_ct signature_result_normalized = signature_result.normalize();
    builder.assert_equal(signature_result_normalized.witness_index, input.result);
}

// Add dummy constraints for ECDSA because when the verifier creates the
// constraint system, they usually use zeroes for witness values.
//
// This does not work for ECDSA as the signature, r, s and public key need
// to be valid.
template <typename Builder> void dummy_ecdsa_constraint(Builder& builder, EcdsaSecp256k1Constraint const& input)
{

    std::array<uint32_t, 32> pub_x_indices_;
    std::array<uint32_t, 32> pub_y_indices_;
    std::array<uint32_t, 64> signature_;
    std::array<uint32_t, 32> message_indices_;

    // Create a valid signature with a valid public key
    crypto::ecdsa_key_pair<secp256k1_ct::fr, secp256k1_ct::g1> account;
    account.private_key = 10;
    account.public_key = secp256k1_ct::g1::one * account.private_key;
    uint256_t pub_x_value = account.public_key.x;
    uint256_t pub_y_value = account.public_key.y;
    std::string message_string = "Instructions unclear, ask again later.";
    crypto::ecdsa_signature signature =
        crypto::ecdsa_construct_signature<crypto::Sha256Hasher, secp256k1_ct::fq, secp256k1_ct::fr, secp256k1_ct::g1>(
            message_string, account);

    // Create new variables which will reference the valid public key and signature.
    // We don't use them in a gate, so when we call assert_equal, they will be
    // replaced as if they never existed.
    for (size_t i = 0; i < 32; ++i) {
        uint32_t m_wit = builder.add_variable(input.hashed_message[i]);
        uint32_t x_wit = builder.add_variable(pub_x_value.slice(248 - i * 8, 256 - i * 8));
        uint32_t y_wit = builder.add_variable(pub_y_value.slice(248 - i * 8, 256 - i * 8));
        uint32_t r_wit = builder.add_variable(signature.r[i]);
        uint32_t s_wit = builder.add_variable(signature.s[i]);
        message_indices_[i] = m_wit;
        pub_x_indices_[i] = x_wit;
        pub_y_indices_[i] = y_wit;
        signature_[i] = r_wit;
        signature_[i + 32] = s_wit;
    }

    // Call assert_equal(from, to) to replace the value in `to` by the value in `from`
    for (size_t i = 0; i < input.hashed_message.size(); ++i) {
        builder.assert_equal(message_indices_[i], input.hashed_message[i]);
    }
    for (size_t i = 0; i < input.pub_x_indices.size(); ++i) {
        builder.assert_equal(pub_x_indices_[i], input.pub_x_indices[i]);
    }
    for (size_t i = 0; i < input.pub_y_indices.size(); ++i) {
        builder.assert_equal(pub_y_indices_[i], input.pub_y_indices[i]);
    }
    for (size_t i = 0; i < input.signature.size(); ++i) {
        builder.assert_equal(signature_[i], input.signature[i]);
    }
}

template void create_ecdsa_k1_verify_constraints<UltraCircuitBuilder>(UltraCircuitBuilder& builder,
                                                                      const EcdsaSecp256k1Constraint& input,
                                                                      bool has_valid_witness_assignments);
template void create_ecdsa_k1_verify_constraints<MegaCircuitBuilder>(MegaCircuitBuilder& builder,
                                                                     const EcdsaSecp256k1Constraint& input,
                                                                     bool has_valid_witness_assignments);
template void dummy_ecdsa_constraint<UltraCircuitBuilder>(UltraCircuitBuilder& builder,
                                                          EcdsaSecp256k1Constraint const& input);

} // namespace acir_format
