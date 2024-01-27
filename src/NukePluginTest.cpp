#include "DDImage/Iop.h"
#include "DDImage/Knobs.h"
#include "DDImage/Row.h"
#include "DDImage/CameraOp.h"
#include "DDImage/Format.h"

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
    FormatPair outFormat;

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
        Button(f, "copytransparams");
        Button(f, "copyanimation");
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

        if (k->is("copytransparams")) 
        {
            copy_create_node_params();
            return 1;
        };

        if (k->is("copyanimation")) 
        {
            copy_cam_animation();
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
    bool test_input(int input, DD::Image::Op* op) const override 
    {
        // When input is 1, we expect it to be a CameraOp
        if (input == 1) {
            // Check if the op is not nullptr before trying to dynamic cast
            if (op == nullptr) {
                // No op is connected, so the input is not valid
                return false;
            }
            // Return true if the dynamic cast is successful, false otherwise
            return dynamic_cast<CameraOp*>(op) != nullptr;
        }

        // For other inputs, call the base class method
        return Iop::test_input(input, op);
    }

    // Validates all the inputs while the node is being used
    // This function will only perform when the output is connected
    void _validate(bool for_real) override 
    {
        // adding copy_info bcs currently i don't need to reformat my footage
        copy_info();

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

    void copy_cam_animation() 
    {
        if (input(0)) input(0)->validate();
        if (input(1)) input(1)->validate();

        Op* cameraOp = input(1);

        if (cameraOp) {
            // Create the camera node
            // knob {curve xframe num}
            // py example: nuke.createNode('Camera2', 'translate {{curve x1 0 x30 1 x60 5 x100 0} {curve x1 0 x30 1 x60 5 x100 0} {curve x1 0 x30 1 x60 5 x100 0}}', False)
            std::stringstream Script;
            Script << "nukescripts.clear_selection_recursive();";
            Script << "cameraNode = nuke.createNode('Camera2', 'translate {{curve x1 0 x30 1 x60 5 x100 0} {curve x1 0 x30 1 x60 5 x100 0} {curve x1 0 x30 1 x60 5 x100 0}}', False);";
            Script << "nuke.autoplace(cameraNode)";
            script_command(Script.str().c_str(), true, false);
            script_unlock();           
        }
    }

    void copy_create_node_params()
    {
        if (input(0)) input(0)->validate();
        if (input(1)) input(1)->validate();

        Op* cameraOp = input(1);

        // validate if camera is connected
        if (cameraOp) {
            // Access to the translate knobs
            Knob* translateKnob = cameraOp->knob("translate");
            // Get Camera transform values
            if (translateKnob) {
                storedTranslate.x = translateKnob->get_value(0);
                storedTranslate.y = translateKnob->get_value(1);
                storedTranslate.z = translateKnob->get_value(2);
            }
            // Run python in nuke to create a node
            std::stringstream Script;
            Script << "nukescripts.clear_selection_recursive();";
            Script << "cameraNode = nuke.createNode('Camera2');";
            Script << "cameraNode['translate'].setValue([" << storedTranslate.x << "," << storedTranslate.y << "," << storedTranslate.z << "]);";
            Script << "nuke.autoplace(cameraNode)";
            script_command(Script.str().c_str(),true,false);
            script_unlock();
        }
    }

    void create_nuke_node()
    {
        // Run python in nuke to create a camera node
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