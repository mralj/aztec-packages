#include <filesystem>
#include <fstream>
#include <iostream>

#include "barretenberg/api/file_io.hpp"
#include "barretenberg/common/net.hpp"
#include "barretenberg/common/thread.hpp"
#include "barretenberg/crypto/sha256/sha256.hpp"
#include "barretenberg/ecc/curves/grumpkin/grumpkin.hpp"

using namespace bb;

const std::string protocol_name = "BARRETENBERG_GRUMPKIN_IPA_CRS";
/**
 * @brief Generates a monomial basis Grumpkin SRS.
 *
 * @details We only provide functionality to create a single transcript file.
 * ! Note that, unlike the bn254 SRS, the first element of the Grumpkin SRS will not be equal to point one defined in
 * grumpkin.hpp as this function finds Grumpkin points in an arbitrary order.
 * As well, unlike the bn254 SRS, the Grumpkin SRS does not need a trusted setup and has no underlying secret generator.
 */
int main(int argc, char** argv)
{
    std::vector<std::string> args(argv, argv + argc);
    if (args.size() <= 1) {
        info("usage: ", args[0], " <subgroup_size> [output_srs_path]");
        return 1;
    }

    const size_t subgroup_size = static_cast<size_t>(atoi(args[1].c_str())); // NOLINT
    const std::filesystem::path srs_path = (args.size() > 2) ? args[2] : "./";
    std::filesystem::create_directories(srs_path);

    std::vector<bb::grumpkin::g1::affine_element> srs(subgroup_size);

    parallel_for_range(subgroup_size, [&](size_t start, size_t end) {
        std::vector<uint8_t> hash_input;
        for (size_t point_idx = start; point_idx < end; ++point_idx) {
            bool rational_point_found = false;
            size_t attempt = 0;
            while (!rational_point_found) {
                hash_input.clear();
                // We hash
                // |BARRETENBERG_GRUMPKIN_IPA_CRS|POINT_INDEX_IN_LITTLE_ENDIAN|POINT_ATTEMPT_INDEX_IN_LITTLE_ENDIAN|
                std::copy(protocol_name.begin(), protocol_name.end(), std::back_inserter(hash_input));
                uint64_t point_index_le_order = htonll(static_cast<uint64_t>(point_idx));
                uint64_t point_attempt_le_order = htonll(static_cast<uint64_t>(attempt));
                hash_input.insert(hash_input.end(),
                                  reinterpret_cast<uint8_t*>(&point_index_le_order),
                                  reinterpret_cast<uint8_t*>(&point_index_le_order) + sizeof(uint64_t));
                hash_input.insert(hash_input.end(),
                                  reinterpret_cast<uint8_t*>(&point_attempt_le_order),
                                  reinterpret_cast<uint8_t*>(&point_attempt_le_order) + sizeof(uint64_t));
                auto hash_result = crypto::sha256(hash_input);
                uint256_t hash_result_uint(
                    ntohll(*reinterpret_cast<uint64_t*>(hash_result.data())),
                    ntohll(*reinterpret_cast<uint64_t*>(hash_result.data() + sizeof(uint64_t))),
                    ntohll(*reinterpret_cast<uint64_t*>(hash_result.data() + 2 * sizeof(uint64_t))),
                    ntohll(*reinterpret_cast<uint64_t*>(hash_result.data() + 3 * sizeof(uint64_t))));
                // We try to get a point from the resulting hash
                auto crs_element = grumpkin::g1::affine_element::from_compressed(hash_result_uint);
                // If the points coordinates are (0,0) then the compressed representation didn't land on an actual point
                // (happens half of the time) and we need to continue searching
                if (!crs_element.x.is_zero() || !crs_element.y.is_zero()) {
                    rational_point_found = true;
                    // Note: there used to be a mutex here, however there is no need as this is just a write to a
                    // computed (exclusive to this thread) memory location
                    srs.at(point_idx) = static_cast<grumpkin::g1::affine_element>(crs_element);
                    break;
                }
                attempt += 1;
            }
        }
    });

    write_file(srs_path / "grumpkin_g1.dat", to_buffer(srs));

    return 0;
}
