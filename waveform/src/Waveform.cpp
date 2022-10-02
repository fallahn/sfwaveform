#include "Waveform.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <xyginext/core/App.hpp>

#include <iostream>

#if defined(__APPLE__)
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#endif

namespace
{

}

Waveform::Waveform()
    : m_file        (),
    m_fileOpen      (false),
    m_vertexHead    (0),
    m_verticalScale (1.f)
{

}

Waveform::~Waveform()
{
    stop();
}

//public
bool Waveform::openFromFile(const std::string& file)
{
    stop();

    if (m_fileOpen = m_file.openFromFile(file); !m_fileOpen)
    {
        return false;
    }

    initialise();

    return true;
}

void Waveform::update()
{
    if (m_fileOpen)
    {
        auto vertexCount = std::min(m_sampleBuffer.size() - m_vertexHead, m_vertices.size() * m_file.getChannelCount());

        const float spacing = static_cast<float>(xy::App::getRenderWindow()->getSize().x) / m_vertices.size();
        const float verticalOffset = (static_cast<float>(xy::App::getRenderWindow()->getSize().y) / 3.f) * 2.f;

        for (auto i = 0u; i < vertexCount; i += m_file.getChannelCount())
        {
            auto index = (i / m_file.getChannelCount());
            m_vertices[index].position.x = spacing * index;
            m_vertices[index].position.y = verticalOffset + ((static_cast<float>(m_sampleBuffer[m_vertexHead + i]) / 70.f) * m_verticalScale);
        }

        m_vertexHead += vertexCount;
    }
}

void Waveform::setVerticalScale(float scale)
{
    m_verticalScale = std::clamp(scale, -1.f, 1.f);
}

//private
bool Waveform::onGetData(sf::SoundStream::Chunk& data)
{
    std::scoped_lock lock(m_mutex);

    std::size_t toFill = m_sampleBuffer.size();
    std::uint64_t currentOffset = m_file.getSampleOffset();

    data.samples = m_sampleBuffer.data();
    data.sampleCount = static_cast<std::size_t>(m_file.read(m_sampleBuffer.data(), toFill));
    currentOffset += data.sampleCount;

    m_vertexHead = 0;

    return (data.sampleCount != 0) && (currentOffset < m_file.getSampleCount());
}

void Waveform::onSeek(sf::Time timeOffset)
{
    std::scoped_lock lock(m_mutex);
    m_file.seek(timeOffset);
}

std::int64_t Waveform::onLoop()
{
    return -1;
}

void Waveform::initialise()
{
    m_sampleBuffer.resize(m_file.getSampleRate() * m_file.getChannelCount());
    m_vertices.resize(m_sampleBuffer.size() / 120);

    SoundStream::initialize(m_file.getChannelCount(), m_file.getSampleRate());
}

void Waveform::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_vertices.data(), m_vertices.size(), sf::LineStrip, states);
}