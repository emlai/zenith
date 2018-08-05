{
    "targets": [
        {
            "includes": [
                "auto.gypi"
            ],
            "include_dirs": [
                "src",
                "<@(SDL2DIR)"
            ],
            "libraries": [
                "<!@(sdl2-config --libs)",
                "-L<@(BOOSTDIR)/lib",
                "-lboost_filesystem"
            ],
            "sources": [
                "src/engine/config.cpp",
                "src/engine/engine.cpp",
                "src/engine/font.cpp",
                "src/engine/graphics.cpp",
                "src/engine/keyboard.cpp",
                "src/engine/math.cpp",
                "src/engine/menu.cpp",
                "src/engine/savefile.cpp",
                "src/engine/sprite.cpp",
                "src/engine/texture.cpp",
                "src/engine/utility.cpp",
                "src/engine/window.cpp",
                "src/main/action.cpp",
                "src/main/ai.cpp",
                "src/main/area.cpp",
                "src/main/component.cpp",
                "src/main/components/dig.cpp",
                "src/main/components/door.cpp",
                "src/main/components/lightsource.cpp",
                "src/main/controller.cpp",
                "src/main/creature.cpp",
                "src/main/entity.cpp",
                "src/main/game.cpp",
                "src/main/gui.cpp",
                "src/main/item.cpp",
                "src/main/liquid.cpp",
                "src/main/main.cpp",
                "src/main/msgsystem.cpp",
                "src/main/object.cpp",
                "src/main/tile.cpp",
                "src/main/world.cpp",
                "src/main/worldgen.cpp"
            ],
            "cflags_cc!": [
                "-fno-rtti"
            ],
            "conditions": [
                ["OS=='mac'", {
                    "xcode_settings": {
                        "GCC_ENABLE_CPP_RTTI": "YES"
                    }
                }]
            ]
        }
    ],
    "includes": [
        "auto-top.gypi"
    ]
}
