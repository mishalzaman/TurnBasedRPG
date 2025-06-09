#include <webgpu/webgpu.h>
#include <core/Core.h>
#include <string>
#include <vector>
#include <variant>
#include <glm/glm.hpp>
#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)
#include <stdexcept>
#include <iostream>
#include <array>
#include <cstring> // For memset

namespace WGPU {
    namespace Buffer {

        /*============================================================
        * BUFFER DATA
        =============================================================*/
        /**
         * @struct BufferData
         * @brief Represents data stored in a GPU buffer for use with WebGPU.
         *
         * This structure is designed to handle different types of data compatible
         * with WebGPU, including scalar types, vectors, and matrices. It includes
         * metadata about the buffer (name, size, and offset) and a variant to store
         * the value in various formats.
         */
        struct BufferData {
            /**
             * @brief The name of the buffer data.
             * This is typically used to identify the purpose or variable associated
             * with this buffer entry.
             */
            std::string name;

            /**
             * @brief The size of the buffer data in bytes.
             * This represents the total size of the data stored in the GPU buffer.
             */
            size_t size;

            /**
             * @brief The offset of the buffer data in bytes.
             * This is the starting position of the data in the GPU buffer.
             */
            size_t offset;

            /**
             * @brief A variant type to store WebGPU-compatible data types.
             *
             * The possible types include:
             * - `float`: A single scalar value.
             * - `int`: A single integer value.
             * - `uint32_t`: A single unsigned integer value.
             * - `std::array<float, 2>`: A 2D vector (vec2).
             * - `std::array<float, 3>`: A 3D vector (vec3).
             * - `std::array<float, 4>`: A 4D vector (vec4).
             * - `std::array<float, 8>`: Padding for alignment or other purposes.
             * - `std::array<float, 12>`: Padding for alignment or other purposes.
             * - `std::array<float, 16>`: A 4x4 matrix (mat4).
             */
            using ValueType = std::variant<
                float,
                int,
                uint32_t,
                std::array<float, 2>,  // For vec2
                std::array<float, 3>,  // For vec3
                std::array<float, 4>,  // For vec4
                std::array<float, 8>,  // Padding
                std::array<float, 12>, // Padding
                std::array<float, 16>  // For mat4
            >;

            /**
             * @brief The value stored in the buffer.
             * This can hold any type defined in the ValueType variant.
             */
            ValueType value;

            /**
             * @brief Default constructor.
             * Initializes the buffer data with default values.
             */
            BufferData()
                : name(""), size(0), offset(0), value() {}

            /**
             * @brief Constructor with parameters.
             *
             * @param name The name of the buffer data.
             * @param size The size of the buffer data in bytes.
             * @param offset The offset of the buffer data in bytes.
             * @param value The value to initialize the buffer data.
             */
            BufferData(const std::string& name, size_t size, size_t offset, const ValueType& value)
                : name(name), size(size), offset(offset), value(value) {}
        };

        class UniformBuffer {
        public:
            UniformBuffer();
            ~UniformBuffer();

            /*============================================================
            * PUBLIC
            =============================================================*/

            WGPUBuffer Get() const { return uniformBuffer_; }

            /**
             * @brief Adds a new uniform to the buffer, ensuring proper alignment and padding.
             *
             * This method handles adding a new uniform value to the buffer, including any
             * necessary padding to maintain proper alignment based on the WebGPU alignment rules.
             * It calculates the total size of the buffer, adjusts offsets, and ensures correct
             * memory layout.
             *
             * @tparam T The type of the value being added to the buffer.
             * @param name The name of the uniform variable.
             * @param value The value of the uniform variable to add.
             */
            template <typename T>
            void Add(const std::string& name, const T& value) {
				// Handle padding on stride prior to adding the new uniform
                BufferData padding = getPadding<T>();
                if (padding.size > 0) {
					totalSize_ += padding.size;
					bufferData_.emplace_back(padding);
                }
                
				// Add the new uniform
                BufferData uniform = convertToStandardFormat(name, value);
                uniform.offset = totalSize_;
                bufferData_.emplace_back(uniform);

                totalSize_ += uniform.size;
            }

            /**
             * @brief Updates the value of an existing uniform in the buffer.
             *
             * This method searches for a uniform by its name and updates its value if the
             * uniform exists and the type matches. If the uniform does not exist or the
             * type does not match, an exception is thrown.
             *
             * @tparam T The type of the value to update.
             * @param name The name of the uniform to update.
             * @param value The new value to set for the uniform.
             * @throws std::invalid_argument If the uniform is not found or if there is a type mismatch.
             */
            template <typename T>
            void Update(const std::string& name, const T& value) {
                for (auto& data : bufferData_) {
                    if (data.name == name) {
                        // Convert value into the correct variant type
                        auto convertedValue = convertToStandardFormat(name, value).value;

                        // Check if the stored type matches the converted type
                        if (data.value.index() == convertedValue.index()) {
                            data.value = convertedValue;
                            return;
                        }
                        else {
                            throw std::invalid_argument("Type mismatch for uniform buffer value.");
                        }
                    }
                }
                throw std::invalid_argument("Uniform buffer not found.");
            }



            /**
             * @brief Writes the uniform buffer data to the GPU.
             *
             * This method checks if the WebGPU device and queue are initialized, creates or updates
             * the uniform buffer if needed, and writes the current uniform buffer data to the GPU.
             *
             * @throws std::runtime_error If the WebGPU device or queue is not initialized, or if
             * the uniform buffer creation fails.
             */
            void Write();

			size_t GetCurrentBufferSize() { return totalSize_; }
        private:


            /*============================================================
            * PRIVATE PROPERTIES
            =============================================================*/

            WGPUBuffer uniformBuffer_ = nullptr;
            size_t totalSize_ = 0;
            std::vector<BufferData> bufferData_;
			WGPUBufferDescriptor bufferDesc_ = {};


            /*============================================================
            * PRIVATE METHODS
            =============================================================*/

            template <typename T>
            BufferData convertToStandardFormat(const std::string& name, const T& value) {
                using ValueType = typename BufferData::ValueType; // Use the correct variant type from BufferData

                if constexpr (
                    std::is_same_v<T, float> || 
                    std::is_same_v<T, int> || 
                    std::is_same_v<T, uint32_t>
                )  {
                    return BufferData(name, sizeof(T), 0, value);
                }
                else if constexpr (std::is_same_v<T, glm::vec2>) {
                    std::array<float, 2> converted = { value.x, value.y };
                    return BufferData(name, sizeof(converted), 0, converted);
                }
                else if constexpr (std::is_same_v<T, glm::vec3>) {
                    std::array<float, 3> converted = { value.x, value.y, value.z };
                    return BufferData(name, sizeof(converted), 0, converted);
                }
                else if constexpr (std::is_same_v<T, glm::vec4>) {
                    std::array<float, 4> converted = { value.x, value.y, value.z, value.w };
                    return BufferData(name, sizeof(converted), 0, converted);
                }
                else if constexpr (std::is_same_v<T, glm::mat4>) {
                    std::array<float, 16> converted;
                    const float* ptr = glm::value_ptr(value); // Extract matrix data as a float array
                    std::copy(ptr, ptr + 16, converted.begin());
                    return BufferData(name, sizeof(converted), 0, converted);
                }
                else {
                    throw std::invalid_argument("Unsupported data type.");
                }
            }


            template <typename T>
            size_t getAlignment() const {
                if constexpr (
                    std::is_same_v<T, float> || 
                    std::is_same_v<T, int> || 
                    std::is_same_v<T, uint32_t>
                )  {
                    return 4; // Scalars are 4-byte aligned
                }
                else if constexpr (std::is_same_v<T, glm::vec2>) {
                    return 8; // vec2 is 8-byte aligned
                }
                else if constexpr (std::is_same_v<T, glm::vec3> || std::is_same_v<T, glm::vec4>) {
                    return 16; // vec3 and vec4 are both 16-byte aligned, altough vec3 is stored with 12 bytes
                }
                else if constexpr (std::is_same_v<T, glm::mat4>) {
                    return 16; // mat4 elements are 16-byte aligned (each column is vec4)
                }
                else {
                    throw std::invalid_argument("Unsupported data type.");
                }
            }

            template <std::size_t N>
            BufferData createPadding(size_t padding) {
                std::array<float, N> paddingValue = {}; // All elements initialized to 0

                // Create and return the BufferData for padding
                return BufferData(
                    "padding_" + std::to_string(bufferData_.size()), // Name
                    padding,                                         // Size of the padding
                    totalSize_,                                      // Offset for the padding
                    paddingValue                                     // ValueType-compatible
                );
            }

			template <typename T>
            BufferData getPadding() {
                // Get the alignment for the current type
                std::size_t alignment = getAlignment<T>();

                // Calculate padding size
                std::size_t padding = 0;
                if (totalSize_ % alignment != 0) {
                    padding = alignment - (totalSize_ % alignment);
                }

                if (padding == 0) {
                    return BufferData();
                }

                if (padding == 4) {
                    return createPadding<4>(padding);
                }
                else if (padding == 8) {
                    return createPadding<8>(padding);
                }
                else if (padding == 12) {
                    return createPadding<12>(padding);
                }
                else {
                    throw std::runtime_error("Unsupported padding size.");
                }
            }
        };
    }
}
