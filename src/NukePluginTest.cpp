#include "DDImage/Iop.h"
#include "DDImage/Knobs.h"
#include "DDImage/Row.h"
#include "DDImage/CameraOp.h"

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

class NukePluginTest : public  Iop {
    public:
    // Constructor
    NukePluginTest(Node* node) : Iop(node) {}

    // Global Vectors and Vars
    Vector3 storedTranslate;

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
    void knobs(Knob_Callback f) override 
    {
        Button(f, "qtdialog");
        Button(f, "createnode");
        Button(f, "copynodeparams");
        SetFlags(f, DD::Image::Knob::STARTLINE);

        Tab_knob(f, "info");
    };

    // Perform action when the button is pressed
    int knob_changed(DD::Image::Knob* k) override 
    {
        if (k->is("qtdialog")) 
        {
            show_qt_dialog();
            return 1;
        };

        if (k->is("copynodeparams")) 
        {
            copy_create_node_params();
            return 1;
        };

        if (k->is("createnode"))
        {
            create_nuke_node();
            return 1;
        }
        return 0;
    }

    // create node inputs
    const char* input_label(int n, char*) const override
    {
    	switch (n) 
        {
            case 0: return "Source";
            case 1: return "Camera";
            default: return "";
        }
    }

    // validate inputs
    // this validates when the user connect the input of this node to a desired input
    // then it checks that the connected input1 ("Camera") corresponds to a Camera node
    // last it checks that the connected input0 ("Source") corresponds to a any 2d node (Iop)
    bool test_input(int input, DD::Image::Op *op) const override
    {
        if (input==1)
        {
            return (dynamic_cast<CameraOp*>(op) != nullptr);
        }
        return Iop::test_input(input, op);
    }

    // Validates all the inputs that the node is using
    void _validate(bool for_real) override {
        // Call the validate base class
        Iop::_validate(for_real);

        // Check if there is a camera connected to input1
        Op* cameraOp = input(1);

        // validate if camera is connected
        if (cameraOp) {
            // Access to the transform knob
            Knob* translateKnob = cameraOp->knob("translate");
            // Get Camera transform values
            if (translateKnob) {
                storedTranslate.x = translateKnob->get_value(0);
                storedTranslate.y = translateKnob->get_value(1);
                storedTranslate.z = translateKnob->get_value(2);
            }
        }
    }

    void copy_create_node_params()
    {
        // Run python in nuke to create a node
        std::stringstream Script;
        Script << "nukescripts.clear_selection_recursive();";
        Script << "cameraNode = nuke.createNode('Camera2');";
        Script << "cameraNode['translate'].setValue([" << storedTranslate.x << "," << storedTranslate.y << "," << storedTranslate.z << "]);";
        Script << "nuke.autoplace(cameraNode)";
        script_command(Script.str().c_str(),true,false);
        script_unlock();
    }

    void create_nuke_node()
    {
        // Run python in nuke to create a node
        std::stringstream Script;
        Script << "nukescripts.clear_selection_recursive();";
        Script << "cameraNode = nuke.createNode('Camera2');";
        Script << "nuke.autoplace(cameraNode)";
        script_command(Script.str().c_str(),true,false);
        script_unlock();
    }

    void show_qt_dialog() 
    {
        // Create the dialog
        QDialog dialogQT;
        dialogQT.setWindowTitle("Hello World!");
        dialogQT.setWindowFlags(Qt::Dialog);

        // Create the label
        QLabel helloWorldLabel(QObject::tr("Hello World!"));

        // Create and set the layout
        QGridLayout gridLayoutQT(&dialogQT);
        gridLayoutQT.addWidget(&helloWorldLabel, 1, 0);

        // Display the dialog and check the result
        if (dialogQT.exec() == QDialog::Rejected) return;
    };

    // Return the plugin required classes
    static const Iop::Description desc;
    int maximum_inputs() const override { return 2; }
    int minimum_inputs() const override { return 2; }
    const char* Class() const override { return CLASS; }
    const char* node_help() const override { return HELP; }
};

// Register the plugin in Nuke
static Iop* build(Node* node) {
    return new NukePluginTest(node);
}
const  DD::Image::Iop::Description NukePluginTest::desc("NukePluginTest", "NukePluginTest", build);