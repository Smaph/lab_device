/**
 * @file device.cpp
 *
 * @brief A C++ program demonstrating the usage of the Stream and Device classes.
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>

using namespace std;

int streamcounter = 0; ///< Global variable to keep track of stream creation.

const int MIXER_OUTPUTS = 1;
const float POSSIBLE_ERROR = 0.01;

/**
 * @class Stream
 * @brief Represents a chemical stream with a name and mass flow.
 */
class Stream
{
private:
    double mass_flow = 0.0; ///< The mass flow rate of the stream.
    string name;      ///< The name of the stream.

public:
    /**
     * @brief Constructor to create a Stream with a unique name.
     * @param s An integer used to generate a unique name for the stream.
     */
    Stream(int s) { 
        setName("s" + std::to_string(s)); 
    }

    /**
     * @brief Set the name of the stream.
     * @param s The new name for the stream.
     */
    void setName(string s) { name = s; }

    /**
     * @brief Get the name of the stream.
     * @return The name of the stream.
     */
    string getName() { return name; }

    /**
     * @brief Set the mass flow rate of the stream.
     * @param m The new mass flow rate value.
     */
    void setMassFlow(double m) { mass_flow = m; }

    /**
     * @brief Get the mass flow rate of the stream.
     * @return The mass flow rate of the stream.
     */
    double getMassFlow() const { return mass_flow; }

    /**
     * @brief Print information about the stream.
     */
    void print() { cout << "Stream " << getName() << " flow = " << getMassFlow() << endl; }
};

/**
 * @class Device
 * @brief Represents a device that manipulates chemical streams.
 */
class Device
{
protected:
    vector<shared_ptr<Stream>> inputs;  ///< Input streams connected to the device.
    vector<shared_ptr<Stream>> outputs; ///< Output streams produced by the device.
    int inputAmount = 0;
    int outputAmount = 0;
    bool calculated;  // true - аппарат рассчитан, false - не рассчитан
    
public:
    Device() : calculated(false) {}
    virtual ~Device() = default;
    
    /**
     * @brief Add an input stream to the device.
     * @param s A shared pointer to the input stream.
     */
    virtual void addInput(shared_ptr<Stream> s) {
        if (inputAmount > 0 && inputs.size() >= inputAmount) {
            throw string("INPUT STREAM LIMIT!");
        }
        inputs.push_back(s);
    }
    
    /**
     * @brief Add an output stream to the device.
     * @param s A shared pointer to the output stream.
     */
    virtual void addOutput(shared_ptr<Stream> s) {
        if (outputAmount > 0 && outputs.size() >= outputAmount) {
            throw string("OUTPUT STREAM LIMIT!");
        }
        outputs.push_back(s);
    }

    // Геттеры для доступа к protected полям (ИСПРАВЛЕНИЕ ОШИБКИ!)
    const vector<shared_ptr<Stream>>& getInputs() const { return inputs; }
    const vector<shared_ptr<Stream>>& getOutputs() const { return outputs; }
    
    // Методы для получения количества потоков
    int getInputCount() const { return inputs.size(); }
    int getOutputCount() const { return outputs.size(); }

    /**
     * @brief Update the output streams of the device (to be implemented by derived classes).
     */
    virtual void updateOutputs() = 0;

    void setCalculated(bool calc) { calculated = calc; }
    bool isCalculated() const { return calculated; }
};

class Mixer : public Device
{
private:
    int _inputs_count = 0;
    
public:
    Mixer(int inputs_count) : Device() {
        _inputs_count = inputs_count;
        inputAmount = inputs_count;
        outputAmount = MIXER_OUTPUTS;
    }
    
    void addInput(shared_ptr<Stream> s) override {
        if (inputs.size() >= _inputs_count) {
            throw string("Too much inputs");
        }
        inputs.push_back(s);
    }
    
    void addOutput(shared_ptr<Stream> s) override {
        if (outputs.size() >= MIXER_OUTPUTS) {
            throw string("Too much outputs");
        }
        outputs.push_back(s);
    }
    
    void updateOutputs() override {
        double sum_mass_flow = 0;
        for (const auto& input_stream : inputs) {
            sum_mass_flow += input_stream->getMassFlow();
        }

        if (outputs.empty()) {
            throw string("Should set outputs before update");
        }

        double output_mass = sum_mass_flow / outputs.size();

        for (auto& output_stream : outputs) {
            output_stream->setMassFlow(output_mass);
        }
    }
};

void shouldSetOutputsCorrectlyWithOneOutput() {
    streamcounter = 0;
    Mixer d1(2);
    
    auto s1 = make_shared<Stream>(++streamcounter);
    auto s2 = make_shared<Stream>(++streamcounter);
    auto s3 = make_shared<Stream>(++streamcounter);
    
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);

    d1.addInput(s1);
    d1.addInput(s2);
    d1.addOutput(s3);

    d1.updateOutputs();

    if (abs(s3->getMassFlow() - 15.0) < POSSIBLE_ERROR) {
        cout << "Test 1 passed" << endl;
    } else {
        cout << "Test 1 failed" << endl;
    }
}

void shouldCorrectOutputs() {
    streamcounter = 0;
    Mixer d1(2);
    
    auto s1 = make_shared<Stream>(++streamcounter);
    auto s2 = make_shared<Stream>(++streamcounter);
    auto s3 = make_shared<Stream>(++streamcounter);
    auto s4 = make_shared<Stream>(++streamcounter);
    
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);

    d1.addInput(s1);
    d1.addInput(s2);
    d1.addOutput(s3);

    try {
        d1.addOutput(s4);
    } catch (const string& ex) {
        if (ex == "Too much outputs") {
            cout << "Test 2 passed" << endl;
            return;
        }
    }

    cout << "Test 2 failed" << endl;
}

void shouldCorrectInputs() {
    streamcounter = 0;
    Mixer d1(2);
    
    auto s1 = make_shared<Stream>(++streamcounter);
    auto s2 = make_shared<Stream>(++streamcounter);
    auto s3 = make_shared<Stream>(++streamcounter);
    auto s4 = make_shared<Stream>(++streamcounter);
    
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);

    d1.addInput(s1);
    d1.addInput(s2);
    d1.addOutput(s3);

    try {
        d1.addInput(s4);
    } catch (const string& ex) {
        if (ex == "Too much inputs") {
            cout << "Test 3 passed" << endl;
            return;
        }
    }

    cout << "Test 3 failed" << endl;
}

class Reactor : public Device {
public:
    Reactor(bool isDoubleReactor) : Device() {
        inputAmount = 1;
        outputAmount = isDoubleReactor ? 2 : 1;
    }
    
    void updateOutputs() override {
        if (inputs.empty()) {
            throw string("No input stream");
        }
        if (outputs.size() != outputAmount) {
            throw string("Wrong number of outputs");
        }
        
        double inputMass = inputs.at(0)->getMassFlow();
        for (int i = 0; i < outputAmount; i++) {
            double outputLocal = inputMass / outputAmount;
            outputs.at(i)->setMassFlow(outputLocal);
        }
    }
};

void testTooManyOutputStreams() {
    streamcounter = 0;
    
    Reactor dl(false);
    
    auto s1 = make_shared<Stream>(++streamcounter);
    auto s2 = make_shared<Stream>(++streamcounter);
    auto s3 = make_shared<Stream>(++streamcounter);
    
    s1->setMassFlow(10.0);
    
    dl.addInput(s1);
    dl.addOutput(s2);
    
    try {
        dl.addOutput(s3);
        cout << "Test 1 failed" << endl;
    } catch (const string& ex) {
        if (ex == "OUTPUT STREAM LIMIT!") {
            cout << "Test 1 passed" << endl;
        } else {
            cout << "Test 1 failed" << endl;
        }
    }
}

void testTooManyInputStreams() {
    streamcounter = 0;
    
    Reactor dl(false);
    
    auto s1 = make_shared<Stream>(++streamcounter);
    auto s2 = make_shared<Stream>(++streamcounter);  // ИСПРАВЛЕНО: s2 объявлен!
    auto s3 = make_shared<Stream>(++streamcounter);
    
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);  // ТЕПЕРЬ РАБОТАЕТ
    
    dl.addInput(s1);
    
    try {
        dl.addInput(s3);
        cout << "Test 2 failed" << endl;
    } catch (const string& ex) {
        if (ex == "INPUT STREAM LIMIT!") {
            cout << "Test 2 passed" << endl;
        } else {
            cout << "Test 2 failed" << endl;
        }
    }
}

void testInputEqualOutput() {
    streamcounter = 0;
    
    Reactor dl(true);
    
    auto s1 = make_shared<Stream>(++streamcounter);
    auto s2 = make_shared<Stream>(++streamcounter);
    auto s3 = make_shared<Stream>(++streamcounter);
    
    s1->setMassFlow(10.0);
    
    dl.addInput(s1);
    dl.addOutput(s2);
    dl.addOutput(s3);
    
    dl.updateOutputs();
    
    // ИСПРАВЛЕНО: используем геттеры и -> вместо .
    double sumOutputs = dl.getOutputs().at(0)->getMassFlow() + 
                        dl.getOutputs().at(1)->getMassFlow();
    double inputMass = dl.getInputs().at(0)->getMassFlow();
    
    if (abs(sumOutputs - inputMass) < POSSIBLE_ERROR) {
        cout << "Test 3 passed" << endl;
    } else {
        cout << "Test 3 failed" << endl;
    }
}

void tests() {
    cout << "========== RUNNING TESTS ==========\n\n";
    
    testInputEqualOutput();
    testTooManyOutputStreams();
    testTooManyInputStreams();
    
    shouldSetOutputsCorrectlyWithOneOutput();
    shouldCorrectOutputs();
    shouldCorrectInputs();
    
    cout << "\n========== TESTS COMPLETE ==========\n";
}

/**
 * @brief The entry point of the program.
 * @return 0 on successful execution.
 */
int main()
{
    streamcounter = 0;
    tests();
    return 0;
}