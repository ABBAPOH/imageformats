import qbs.base 1.0

Product {
    type: "application"
    name: "tst_icns"
    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: ["core", "gui", "test"] }
    files: [
        "tst_icns.cpp",
        "tst_icns.qrc"
    ]
}
