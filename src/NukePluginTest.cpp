#include "DDImage/Iop.h"
#include "DDImage/Knobs.h"
#include "DDImage/Row.h"

#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>

#include "ToggleButton.h"

using namespace DD::Image;

const char* CLASS = "NukePluginTest";
const char* HELP = "A basic nuke plugin for nuke.";

class NukePluginTest : public Iop {
    public:
    // Constructor
    NukePluginTest(Node* node) : Iop(node) {}

    // Required Method
    void _validate(bool) override {
        // set the output image size and format
        copy_info();
    }

    void _request(int x, int y, int r, int t, ChannelMask channels,  int count) override {
        // request the input image data
        input(0)->request(x, y, r, t, channels, count);
    }

    void engine(int y, int x, int r, ChannelMask channels, Row& row) override {
        // Get input0 image
        row.get(input0(), y, x, r, channels);

        // Process each pixel of the image
        foreach(z, channels) {
            float* outptr = row.writable(z);
            for (int i = x; i < r; i++) {
                // simple operation (set's rgba channels to white)
                outptr[i] = 1.0f;
            }
        }
    }

    // Define the plugin knobs
    void knobs(Knob_Callback f) override {
        Button(f, "qtmodal");
    };

    int knob_changed(Knob* k) {
        if (k->name() == "qtmodal") {
            show_dialog();
            return 1;
        }
        return 0;
    }

    void show_dialog() {

        QDialog DialogQT;

        DialogQT.setWindowTitle("Hello World!");
        DialogQT.setWindowFlags(Qt::Dialog);

        DialogQT.exec();
    }

    // Return the plugin required classes
    static const Iop::Description desc;
    const char* Class() const override { return CLASS; }
    const char* node_help() const override { return HELP; }
};

// Register the plugin in Nuke
static Iop* build(Node* node) {
    return new NukePluginTest(node);
}
const Iop::Description NukePluginTest::desc("NukePluginTest", "3D/NukePluginTest", build);