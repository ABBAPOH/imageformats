import qbs.base 1.0

Product {
    type: "application"
    name: "tst_qicns"
    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: ["core", "gui", "test"] }
    files: [
        "tst_qicns.cpp",
        "tst_qicns.qrc"
    ]
}
