#pragma once

#include <SFML/Audio/Export.hpp>
#include <SFML/Audio/SoundStream.hpp>
#include <SFML/Audio/InputSoundFile.hpp>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Vertex.hpp>

#include <string>
#include <vector>
#include <mutex>

class Waveform final : public sf::SoundStream, public sf::Drawable
{
public:
    Waveform();
    ~Waveform() override;

    bool openFromFile(const std::string&);

    void update();


private:

    sf::InputSoundFile m_file;
    std::vector<std::int16_t> m_sampleBuffer;
    std::recursive_mutex m_mutex;

    [[nodiscard]] bool onGetData(sf::SoundStream::Chunk&) override;
    void onSeek(sf::Time) override;
    sf::Int64 onLoop() override;
    void initialise();

    bool m_fileOpen;
    std::vector<sf::Vertex> m_vertices;
    std::size_t m_vertexHead;
    void draw(sf::RenderTarget&, sf::RenderStates) const override;
};