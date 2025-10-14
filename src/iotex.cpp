#include <vector>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <memory>
#include "ngph/iotex.h"
#include "ngph/texture.h"
#include "grid.h"

namespace gph {
    
    // initialize TexTable
    TexTable::TexTable() {}

    Texture TexTable::getTexture(const std::string& texName) const {
        auto it = this->textures.find(texName);

        if (it != this->textures.end()) {
            return *it->second; 
        }

        return Texture::Builder().build(); 
    }

    const Texture& TexTable::getTextureRef(const std::string& texName) const {
        auto it = this->textures.find(texName);

        if (it != this->textures.end()) {
            return *it->second; 
        }

        throw std::runtime_error("Can't reference to a non-existent texture with a name " + texName);
    }

    void TexTable::setTexture(const std::string& texName, const Texture& texture) {
        this->textures.insert({texName, std::make_unique<Texture>(std::move(texture))});
    }
    
    void TexTable::delTexture(const std::string& texName) {
        this->textures.erase(texName);
    }

    std::vector<char> TexTable::serialize() {
        std::vector<char> buffer;

        auto append = [&](const void* data, std::size_t size) {
            const char* bytes = reinterpret_cast<const char*>(data);
            buffer.insert(buffer.end(), bytes, bytes + size);
        };

        size_t mapSize = this->textures.size();
        append(&mapSize, sizeof(mapSize));

        for (auto& [key, texturePtr] : this->textures) {
            size_t keyLen = key.size();
            append(&keyLen, sizeof(keyLen));
            append(key.data(), keyLen);

            GridBuffer texBuffer = texturePtr->newBuffer();
            size_t texSize = texBuffer.getSize();
            append(&texSize, sizeof(texSize));
            append(texBuffer.buffer.data(), texSize);
        }

        return buffer;
    }

    void TexTable::deserialize(std::vector<char> buffer) {
        size_t offset = 0;

        auto read = [&](void* data, std::size_t size) {
            std::memcpy(data, buffer.data() + offset, size);
            offset += size;
        };

        size_t mapSize = 0;
        read(&mapSize, sizeof(mapSize));

        for (size_t i = 0; i < mapSize; ++i) {
            size_t keyLen;
            read(&keyLen, sizeof(keyLen));

            std::string key(keyLen, '\0');
            read(key.data(), keyLen);

            size_t texSize;
            read(&texSize, sizeof(texSize));

            std::vector<char> texData(texSize);
            read(texData.data(), texSize);

            GridBuffer texBuffer(texData);
            Grid grid = texBuffer.unpack();
            Texture texture = Texture::Builder()
                .setGrid(grid)
                .build();

            this->setTexture(key, texture);
        }
    }
    
    void TexTable::loadTable(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) return;

        std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        // deserialize method will automatically update the map via loaded buffer
        this->deserialize(buffer);
    }

    void TexTable::saveTable(const std::string& path) {
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        if (!file) return;

        // serializes current texture map, then writes it into the path
        std::vector<char> buffer = this->serialize();
        file.write(buffer.data(), buffer.size());
        file.close();
    }
}