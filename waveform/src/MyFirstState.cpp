/*********************************************************************
(c) Jonny Paton 2018

xygineXT - Zlib license.

This software is provided 'as-is', without any express or
implied warranty. In no event will the authors be held
liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute
it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented;
you must not claim that you wrote the original software.
If you use this software in a product, an acknowledgment
in the product documentation would be appreciated but
is not required.

2. Altered source versions must be plainly marked as such,
and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any
source distribution.
*********************************************************************/

#include "MyFirstState.hpp"
#include "States.hpp"

#include <xyginext/gui/Gui.hpp>
#include <xyginext/core/FileSystem.hpp>
#include <xyginext/graphics/postprocess/Bloom.hpp>
#include <xyginext/graphics/postprocess/Blur.hpp>

#include <xyginext/ecs/systems/SpriteSystem.hpp>
#include <xyginext/ecs/systems/CameraSystem.hpp>
#include <xyginext/ecs/systems/RenderSystem.hpp>

#include <xyginext/ecs/components/Sprite.hpp>
#include <xyginext/ecs/components/Drawable.hpp>
#include <xyginext/ecs/components/Camera.hpp>
#include <xyginext/ecs/components/Transform.hpp>

MyFirstState::MyFirstState(xy::StateStack& ss, xy::State::Context ctx) 
    : xy::State (ss, ctx),
    m_scene     (ctx.appInstance.getMessageBus())
{
    registerWindow([&]() 
        {
            if (ImGui::Begin("Window"))
            {
                if (ImGui::Button("Open File"))
                {
                    auto path = xy::FileSystem::openFileDialogue("", "ogg");
                    if (!path.empty())
                    {
                        m_waveform.openFromFile(path);
                    }
                }

                if (ImGui::Button("Play"))
                {
                    m_waveform.play();
                }

                if (ImGui::Button("Stop"))
                {
                    m_waveform.stop();
                }
            }
            ImGui::End();
        });

    buildScene();
}

//public
bool MyFirstState::handleEvent(const sf::Event& evt)
{
    m_scene.forwardEvent(evt);
    return true;
}

void MyFirstState::handleMessage(const xy::Message& msg)
{
    m_scene.forwardMessage(msg);
}

bool MyFirstState::update(float dt)
{
    m_waveform.update();

    m_scene.update(dt);
    return true;
}

void MyFirstState::draw()
{
    m_texture.clear();
    m_texture.draw(m_waveform);
    m_texture.display();

    auto rw = getContext().appInstance.getRenderWindow();
    rw->draw(m_scene);

}

xy::StateID MyFirstState::stateID() const
{
    return States::MyFirstState;
}

//private
void MyFirstState::buildScene()
{
    //TODO this needs to pass its size to the waveform
    //as the waveform update currently uses the window
    //size not the active target size.
    m_texture.create(1920, 1080);

    auto& mb = getContext().appInstance.getMessageBus();
    m_scene.addSystem<xy::SpriteSystem>(mb);
    m_scene.addSystem<xy::CameraSystem>(mb);
    m_scene.addSystem<xy::RenderSystem>(mb);

    //m_scene.addPostProcess<xy::PostBloom>();

    auto entity = m_scene.createEntity();
    entity.addComponent<xy::Transform>();
    entity.addComponent<xy::Drawable>();
    entity.addComponent<xy::Sprite>().setTexture(m_texture.getTexture());
}