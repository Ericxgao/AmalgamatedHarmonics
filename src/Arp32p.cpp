#include "AH.hpp"
#include "AHCommon.hpp"

#include <iostream>

using namespace ah;

struct Pattern2 {
	
	std::vector<int> notes;
	unsigned int nNotes = 0;

	unsigned int patternLength = 0;
	int stepSize = 0;
	unsigned int stepScale = 0;
	unsigned int patternOffset = 0;
	bool repeatLast = false;

	unsigned int index = 0;

	unsigned int MAJOR[7] = {0,2,4,5,7,9,11};
	unsigned int MINOR[7] = {0,2,3,5,7,8,10};
		
	virtual const char* getName() const = 0;

	// TODO: Unit tests for Pattern2::initialise and derived class overrides:
	// - Test correct population of 'notes' vector for each pattern type (Diverge, Converge, Return, Bounce, Rez, OnTheRun)
	//   across various lengths, step sizes, scales, and repeatLast settings.
	// - Verify 'nNotes' is correctly set to notes.size().
	// - Verify 'index' is set to 'patternOffset % nNotes'.
	// - Verify 'patternOffset' is correctly applied (e.g. patternOffset = patternOffset % nNotes).
	// - Verify notes.capacity() is >= expected reservation (MAX_STEPS * 2 for dynamic, specific for Rez/OnTheRun).
	virtual void initialise(unsigned int _length, unsigned int _scale, int _size, unsigned int _offset, bool _repeat) {
		notes.reserve(Arp32::MAX_STEPS * 2); // Max length for Return/Bounce can be 2*patternLength
		patternLength = _length;
		stepSize = _size;
		stepScale = _scale;
		patternOffset = _offset;
		repeatLast = _repeat;
	};

	void advance() {
		// std::cout << "ADV " << index << std::endl;
		index++;
	};

	void reset() {
		// std::cout << "RES" << std::endl;
		index = patternOffset;
	}

	// TODO: Unit tests for getOffset:
	// - Test with empty notes vector (should return 0).
	// - Test with index out of bounds (e.g., index >= notes.size()) (should return 0).
	// - Test with valid index and notes (should return notes[index]).
	int getOffset() {
		// std::cout << "OUT " << index << " " << notes[index] << std::endl;
		if (notes.empty()) {
			return 0;
		}
		if (index >= notes.size()) {
			return 0;
		}
		return notes[index];
	}

	bool isPatternFinished() {
		// std::cout << "FIN " << index << " " << nNotes << std::endl;
		return (index >= nNotes - 1); 
	}

	int getMajor(int count) {
		int i = abs(count);
		int sign = (count < 0) ? -1 : (count > 0);
		return sign * ((i / 7) * 12 + MAJOR[i % 7]);
	}

	int getMinor(int count) {
		int i = abs(count);
		int sign = (count < 0) ? -1 : (count > 0);
		return sign * ((i / 7) * 12 + MINOR[i % 7]);
	}

	// TODO: Unit tests for randomize:
	// - Test behavior when length (nNotes - patternOffset) is zero or negative (should return early).
	// - Test normal operation:
	//   - Verify that two different elements in 'notes' (within the range [patternOffset, nNotes-1]) are swapped.
	//   - Verify that if p1 == p2 initially, an attempt is made to find a different p2.
	void randomize() {
		int length = nNotes - patternOffset;

#ifndef METAMODULE
		if (length <= 0) {
			std::cout << "randomize() called with non-positive length, returning." << std::endl;
			return;
		}
#endif

		int p1 = (rand() % length) + patternOffset;
		int p2 = (rand() % length) + patternOffset;
		int tries = 0;

		while (p1 == p2 && tries < 5) { // Make some effort to change the sequence, break after 5 attempts
			p2 = (rand() % length) + patternOffset;
			tries++;
		}

		// std::cout << "RND " << length << " " << p1 << " " << p2 << " "; 

		unsigned int t = notes[p1];
		notes[p1] = notes[p2];
		notes[p2] = t;

		// for (int i = 0; i < nPitches; i++) {
		// 	std::cout << indexes[i];
		// }

		// std::cout << std::endl;

	}

};

struct DivergePattern2 : Pattern2 {

	const char* const name = "Diverge";

	const char* getName() const override {
		return name;
	};

	void initialise(unsigned int _length, unsigned int _scale, int _size, unsigned int _offset, bool _repeat) override {

		Pattern2::initialise(_length, _scale, _size, _offset, _repeat);

		// std::cout << name;
		// std::cout << " DEF ";
		notes.clear();

		for (unsigned int i = 0; i < patternLength; i++) {
			
			int n;
			switch(stepScale) {
				case 0: n = i * stepSize; break;
				case 1: n = getMajor(i * stepSize); break;
				case 2: n = getMinor(i * stepSize); break;
				default:
					n = i * stepSize; break;
			}

			notes.push_back(n);
			// std::cout << n << " ";

		}

		nNotes = notes.size();
		patternOffset = patternOffset % nNotes;
		index = patternOffset;
		// std::cout << " NP=" << nNotes << " -> " << index << std::endl;

	}

};

struct ConvergePattern2 : Pattern2 {

	const char* const name = "Converge";

	const char* getName() const override {
		return name;
	};

	void initialise(unsigned int _length, unsigned int _scale, int _size, unsigned int _offset, bool _repeat) override {

		Pattern2::initialise(_length, _scale, _size, _offset, _repeat);

		// std::cout << name;
		// std::cout << " DEF ";
		notes.clear();

		for (int i = patternLength - 1; i >= 0; i--) {
			
			int n;
			switch(stepScale) {
				case 0: n = i * stepSize; break;
				case 1: n = getMajor(i * stepSize); break;
				case 2: n = getMinor(i * stepSize); break;
				default:
					n = i * stepSize; break;
			}

			notes.push_back(n);
			// std::cout << n << " ";

		}

		nNotes = notes.size();
		patternOffset = patternOffset % nNotes;
		index = patternOffset;
		// std::cout << " NP=" << nNotes << " -> " << index << std::endl;

	}

};

struct ReturnPattern2 : Pattern2 {

	const char* const name = "Return";

	const char* getName() const override {
		return name;
	};

	void initialise(unsigned int _length, unsigned int _scale, int _size, unsigned int _offset, bool _repeat) override {

		Pattern2::initialise(_length, _scale, _size, _offset, _repeat);

		// std::cout << name;
		// std::cout << " DEF ";
		notes.clear();

		for (unsigned int i = 0; i < patternLength; i++) {
			
			int n;
			switch(stepScale) {
				case 0: n = i * stepSize; break;
				case 1: n = getMajor(i * stepSize); break;
				case 2: n = getMinor(i * stepSize); break;
				default:
					n = i * stepSize; break;
			}

			notes.push_back(n);
			// std::cout << n << " ";

		}

		int end = repeatLast ? 0 : 1;

		for (int i = patternLength - 2; i >= end; i--) {
			
			int n;
			switch(stepScale) {
				case 0: n = i * stepSize; break;
				case 1: n = getMajor(i * stepSize); break;
				case 2: n = getMinor(i * stepSize); break;
				default:
					n = i * stepSize; break;
			}

			notes.push_back(n);
			// std::cout << n << " ";
		}

		nNotes = notes.size();
		patternOffset = patternOffset % nNotes;
		index = patternOffset;
		// std::cout << " NP=" << nNotes << " -> " << index << std::endl;

	}

};

struct BouncePattern2 : Pattern2 {

	const char* const name = "Bounce";

	const char* getName() const override {
		return name;
	};

	void initialise(unsigned int _length, unsigned int _scale, int _size, unsigned int _offset, bool _repeat) override {

		Pattern2::initialise(_length, _scale, _size, _offset, _repeat);

		// std::cout << name;
		// std::cout << " DEF ";
		notes.clear();

		for (int i = patternLength - 1; i >= 0; i--) {
			
			int n;
			switch(stepScale) {
				case 0: n = i * stepSize; break;
				case 1: n = getMajor(i * stepSize); break;
				case 2: n = getMinor(i * stepSize); break;
				default:
					n = i * stepSize; break;
			}

			notes.push_back(n);
			// std::cout << n << " ";

		}

		int end = repeatLast ? 0 : 1;

		for (unsigned int i = 1; i < patternLength - end; i++) {
			
			int n;
			switch(stepScale) {
				case 0: n = i * stepSize; break;
				case 1: n = getMajor(i * stepSize); break;
				case 2: n = getMinor(i * stepSize); break;
				default:
					n = i * stepSize; break;
			}

			notes.push_back(n);
			// std::cout << n << " ";

		}

		nNotes = notes.size();
		patternOffset = patternOffset % nNotes;
		index = patternOffset;
		// std::cout << " NP=" << nNotes << " -> " << index << std::endl;

	}

};

struct NotePattern2 : Pattern2 {

	void initialise(unsigned int _length, unsigned int _scale, int _size, unsigned int _offset, bool _repeat) override {

		Pattern2::initialise(_length, _scale, _size, _offset, _repeat);

		nNotes = notes.size();
		patternOffset = patternOffset % nNotes;
		index = patternOffset;

	}

};

struct RezPattern2 : NotePattern2 {

	const char* const name = "Rez";

	const char* getName() const override {
		return name;
	};

	RezPattern2() {
		notes.reserve(17);
		notes.clear();
		notes.push_back(0);
		notes.push_back(12);
		notes.push_back(0);
		notes.push_back(0);
		notes.push_back(8);
		notes.push_back(0);
		notes.push_back(0);
		notes.push_back(3);		
		notes.push_back(0);
		notes.push_back(0);
		notes.push_back(3);
		notes.push_back(0);
		notes.push_back(3);
		notes.push_back(0);
		notes.push_back(8);
		notes.push_back(0);
	}

};

struct OnTheRunPattern2 : NotePattern2 {
	
	const char* const name = "On The Run";

	const char* getName() const override {
		return name;
	};

	OnTheRunPattern2() {
		notes.reserve(9);
		notes.clear();
		notes.push_back(0);
		notes.push_back(4);
		notes.push_back(6);
		notes.push_back(4);
		notes.push_back(9);
		notes.push_back(11);
		notes.push_back(13);
		notes.push_back(11);
	}
	
};

struct Arp32 : core::AHModule {

	const static int MAX_STEPS = 16;
	const static int MAX_DIST = 12; // Octave

	enum ParamIds {
		PATT_PARAM,
		LENGTH_PARAM,
		SIZE_PARAM,
		SCALE_PARAM,
		OFFSET_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		CLOCK_INPUT,
		PITCH_INPUT,
		PATT_INPUT,
		LENGTH_INPUT,
		SIZE_INPUT,
		HOLD_INPUT,
		RANDOM_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_OUTPUT,
		GATE_OUTPUT,
		EOC_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	Arp32() : core::AHModule(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		configParam(PATT_PARAM, 0.0, 5.0, 0.0, "Pattern"); 

		configParam(SIZE_PARAM, -24, 24, 1, "Step size"); 
		getParamQuantity(SIZE_PARAM)->description = "Size of each step in the pattern. Positive = increase pitch, negative = decrease pitch";

		configParam(LENGTH_PARAM, 1.0, 16.0, 1.0, "Number of steps in the pattern");

		configParam(OFFSET_PARAM, 0.0, 10.0, 0.0, "Start offset"); 
		getParamQuantity(OFFSET_PARAM)->description = "Number of steps into the arpeggio to start";

		configSwitch(PATT_PARAM, 0, 5, 0, "Pattern", {"Diverge", "Converge", "Return", "Bounce", "Rez", "On The Run"});

		configSwitch(SCALE_PARAM, 0, 2, 0, "Scale", {"Semitone", "Major", "Minor"}); 
		getParamQuantity(SCALE_PARAM)->description = "Scale of step: semitones, major or minor intervals"; 

		configInput(CLOCK_INPUT, "Clock");
		configInput(PITCH_INPUT, "1V/oct pitch (Poly)");
		configInput(PATT_INPUT, "Pattern selection");
		configInput(SIZE_INPUT, "Step size");
		configInput(HOLD_INPUT, "Trigger: Hold arpeggio");
		configInput(RANDOM_INPUT, "Trigger: Randomize arpeggio pitch order");

		configOutput(OUT_OUTPUT, "1V/oct pitch");
		configOutput(GATE_OUTPUT, "Trigger: On pitch change");
		configOutput(EOC_OUTPUT, "Trigger: On end of pattern");

		patterns.push_back(&patt_diverge);
		patterns.push_back(&patt_converge);
		patterns.push_back(&patt_return);
		patterns.push_back(&patt_bounce);
		patterns.push_back(&patt_rez);
		patterns.push_back(&patt_ontherun);

		nextPattern = patterns[0]->getName();

		onReset();
		id = rand();
		debugFlag = false;
		// Initialize the currently selected pattern with its specific repeatEnd setting
		currPatt->initialise(1, 0, 1, 0, repeatEnd); 
		// Then, ensure all patterns (including the current one again, which is harmless and simpler)
		// are initialized with a default small size and repeatEnd=false to pre-allocate their vectors
		// and populate them with minimal data.
		// TODO: Unit tests for Arp32 constructor's pattern initialization:
		// - Verify currPatt is initialized with correct default parameters (length=1, scale=0, size=1, offset=0, and module's repeatEnd).
		// - Verify all patterns in the 'patterns' vector are initialized with default parameters (length=1, scale=0, size=1, offset=0, repeat=false).
		// - Verify notes vectors in all patterns have their capacity reserved appropriately after this initialization.
		for (Pattern2* patt : patterns) {
			patt->initialise(1, 0, 1, 0, false); 
		}
	}

	void process(const ProcessArgs &args) override;

	void onReset() override {
		isRunning = false;
	}

	json_t *dataToJson() override {
		json_t *rootJ = json_object();

		// gateMode
		json_t *gateModeJ = json_integer((int) gateMode);
		json_object_set_new(rootJ, "gateMode", gateModeJ);

		// repeatMode
		json_t *repeatModeJ = json_boolean((bool) repeatEnd);
		json_object_set_new(rootJ, "repeatMode", repeatModeJ);

		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override {
		// gateMode
		json_t *gateModeJ = json_object_get(rootJ, "gateMode");
		if (gateModeJ) gateMode = (GateMode)json_integer_value(gateModeJ);

		// repeatMode
		json_t *repeatModeJ = json_object_get(rootJ, "repeatMode");
		if (repeatModeJ) repeatEnd = json_boolean_value(repeatModeJ);
	}

	enum GateMode {
		TRIGGER,
		RETRIGGER,
		CONTINUOUS,
	};
	GateMode gateMode = TRIGGER;

	rack::dsp::SchmittTrigger clockTrigger; // for clock
	rack::dsp::SchmittTrigger randomTrigger; // for random

	rack::dsp::PulseGenerator gatePulse;
	rack::dsp::PulseGenerator eocPulse;

	int id = 0;
	float outVolts = 0;
	float rootPitch = 0.0;
	bool isRunning = false;
	bool eoc = false;
	bool repeatEnd = false;

	unsigned int inputPat = 0;
	unsigned int inputLen = 0;
	int inputSize = 0;
	unsigned int inputScale = 0;

	int pattern = 0;
	int length = 0;
	float size = 0;
	float scale = 0;
	int offset = 0;

	std::vector<Pattern2 *>patterns;

	DivergePattern2			patt_diverge; 
	ConvergePattern2 		patt_converge; 
	ReturnPattern2 			patt_return;
	BouncePattern2 			patt_bounce;
	RezPattern2 			patt_rez;
	OnTheRunPattern2		patt_ontherun;

	Pattern2 *currPatt = &patt_diverge;
	const char* nextPattern = "";

};

void Arp32::process(const ProcessArgs &args) {
	
	AHModule::step();

	// Wait a few steps for the inputs to flow through Rack
	if (stepX < 10) { 
		return;
	}

	// Get inputs from Rack
	float clockInput	= inputs[CLOCK_INPUT].getVoltage();
	float clockActive	= inputs[CLOCK_INPUT].isConnected();
	float randomInput	= inputs[RANDOM_INPUT].getVoltage();

	// Read param section	
	if (inputs[PATT_INPUT].isConnected()) {
		inputPat = clamp(static_cast<unsigned int>(inputs[PATT_INPUT].getVoltage()), 0, 5);
	} else {
		inputPat = params[PATT_PARAM].getValue();
	}	

	if (inputs[LENGTH_INPUT].isConnected()) {
		inputLen = clamp(static_cast<unsigned int>(inputs[LENGTH_INPUT].getVoltage()), 1, 16);
	} else {
		inputLen = params[LENGTH_PARAM].getValue();
	}	

	if (inputs[SIZE_INPUT].isConnected()) {
		inputSize = clamp(static_cast<int>(inputs[SIZE_INPUT].getVoltage()), -24, 24);
	} else {
		inputSize = params[SIZE_PARAM].getValue();
	}	

	inputScale = static_cast<int>(params[SCALE_PARAM].getValue());
	offset = static_cast<unsigned int>(params[OFFSET_PARAM].getValue());
	int hold = digital::sgn(inputs[HOLD_INPUT].getVoltage(), 0.001);

	// Process inputs
	bool clockStatus = clockTrigger.process(clockInput);
	bool randomStatus = randomTrigger.process(randomInput);

	// Need to understand why this happens
	if (inputLen == 0) {
		#ifndef METAMODULE
		if (debugEnabled(5000)) { std::cout << stepX << " " << id  << " InputLen == 0, aborting" << std::endl; }
		#endif
		return; // No inputs, no music
	}

	// If there is no clock input, then force that we are not running
	if (!clockActive) {
		isRunning = false;
	}

	bool restart = false;

	// Have we been clocked?
	if (clockStatus) {

		// EOC was fired at last sequence step
		if (eoc) {
			eocPulse.trigger(digital::TRIGGER);
			eoc = false;
		}	
		
		// If we are already running, process cycle
		if (isRunning) {

			#ifndef METAMODULE
			#ifndef METAMODULE
if (debugEnabled()) { std::cout << stepX << " " << id  << " Advance Cycle: " << currPatt->getOffset() << std::endl; }
#endif
			#endif

			// Reached the end of the pattern?
			if (currPatt->isPatternFinished()) {

				// Trigger EOC mechanism
				eoc = true;

				#ifndef METAMODULE
				#ifndef METAMODULE
if (debugEnabled()) { std::cout << stepX << " " << id  << " Finished Cycle" << std::endl; }
#endif
				#endif
				restart = true;

			} 

			// Finally set the out voltage
			outVolts = clamp(rootPitch + music::SEMITONE * (float)currPatt->getOffset(), -10.0f, 10.0f);

			#ifndef METAMODULE	
			#ifndef METAMODULE
if (debugEnabled()) { std::cout << stepX << " " << id  << " Output V = " << outVolts << std::endl; }
#endif
			#endif

			// Pulse the output gate
			gatePulse.trigger(digital::TRIGGER);

			// Completed 1 step
			currPatt->advance();

		} else {

			// Start a cycle
			restart = true;

		}

	}

	// Randomise if triggered
	if (randomStatus && isRunning && hold != -1) {
		currPatt->randomize();
	}

	// If we have been triggered, start a new sequence
	if (restart) {

		if (!hold) {

			// Read input pitch
			float inputPitch;
			if (inputs[PITCH_INPUT].isConnected()) {
				inputPitch = inputs[PITCH_INPUT].getVoltage();
			} else {
				inputPitch = 0.0;
			}

			// At the first step of the cycle
			// So this is where we tweak the cycle parameters
			pattern = inputPat;
			length = inputLen;
			size = inputSize;
			scale = inputScale;

			currPatt = patterns[pattern];

			// Save pitch
			rootPitch = inputPitch;

			#ifndef METAMODULE
			if (debugEnabled()) { std::cout << stepX << " " << id  << 
				" Initiatise new Cycle: Pattern: " << currPatt->getName() << 
				" Length: " << inputLen << std::endl; 
			}
			#endif

			currPatt->initialise(length, scale, size, offset, repeatEnd);

		} else {

			#ifndef METAMODULE
			if (debugEnabled()) { std::cout << stepX << " " << id  << 
				" Hold new Cycle: Pattern: " << currPatt->getName() << 
				" Length: " << inputLen << std::endl; 
			}
			#endif

			currPatt->reset();

		}

		// Start
		isRunning = true;

	} 

	nextPattern = patterns[inputPat]->getName();

	// Set the value
	outputs[OUT_OUTPUT].setVoltage(outVolts);

	bool gPulse = gatePulse.process(args.sampleTime);
	bool cPulse = eocPulse.process(args.sampleTime);

	bool gatesOn = isRunning;
	if (gateMode == TRIGGER) {
		gatesOn = gatesOn && gPulse;
	} else if (gateMode == RETRIGGER) {
		gatesOn = gatesOn && !gPulse;
	}

	outputs[GATE_OUTPUT].setVoltage(gatesOn ? 10.0 : 0.0);
	outputs[EOC_OUTPUT].setVoltage(cPulse ? 10.0 : 0.0);

}

struct Arp32Display : TransparentWidget {

	Arp32 *module;
    std::string fontPath;

    Arp32Display() {
		fontPath = asset::plugin(pluginInstance, "res/RobotoCondensed-Bold.ttf");
    }

	void draw(const DrawArgs &ctx) override {

		if (module == NULL) {
			return;
		}

		Vec pos = Vec(3,14);

		std::shared_ptr<Font> font = APP->window->loadFont(fontPath);

		if (font) {		
			nvgGlobalTint(ctx.vg, color::WHITE);
			nvgFontSize(ctx.vg, 14.5);
			nvgFontFaceId(ctx.vg, font->handle);
			nvgTextLetterSpacing(ctx.vg, -1);

			nvgFillColor(ctx.vg, nvgRGBA(0x00, 0xFF, 0xFF, 0xFF));
		
			char text[128];
			if (module->inputLen == 0) {
				snprintf(text, sizeof(text), "Error: inputLen == 0");
			} else {
				switch(module->inputScale) {
					case 0: 
						snprintf(text, sizeof(text), "%s (%d, %dst, %d)", 
							module->nextPattern,
							module->inputLen,
							module->inputSize,
							module->offset);
						break;
					case 1: 
						snprintf(text, sizeof(text), "%s (%d, %dM, %d)", 
							module->nextPattern,
							module->inputLen,
							module->inputSize,
							module->offset);
						break;
					case 2: 
						snprintf(text, sizeof(text), "%s (%d, %dm, %d)", 
							module->nextPattern,
							module->inputLen,
							module->inputSize,
							module->offset);
						break;
					default: snprintf(text, sizeof(text), "Error..."); break;
				}
			}
			nvgText(ctx.vg, pos.x, pos.y, text, NULL);
		}
	}

};

struct Arp32Widget : ModuleWidget {

	std::vector<MenuOption<Arp32::GateMode>> gateOptions;
	std::vector<MenuOption<bool>> noteOptions;

	Arp32Widget(Arp32 *module) {

		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Arp32p.svg")));

		addParam(createParamCentered<gui::AHKnobSnap>(Vec(28.302, 52.157), module, Arp32::PATT_PARAM));
		addParam(createParamCentered<gui::AHKnobSnap>(Vec(66.865, 52.157), module, Arp32::LENGTH_PARAM));
		addParam(createParamCentered<gui::AHKnobSnap>(Vec(105.07, 52.24), module, Arp32::SIZE_PARAM));
		addParam(createParamCentered<gui::AHKnobSnap>(Vec(22.637, 173.705), module, Arp32::OFFSET_PARAM));
		addParam(createParamCentered<gui::AHKnobSnap>(Vec(53.816, 173.705), module, Arp32::SCALE_PARAM));

		addInput(createInputCentered<gui::AHPort>(Vec(28.302, 90.94), module, Arp32::PATT_INPUT));
		addInput(createInputCentered<gui::AHPort>(Vec(66.865, 90.94), module, Arp32::LENGTH_INPUT));
		addInput(createInputCentered<gui::AHPort>(Vec(105.07, 90.94), module, Arp32::SIZE_INPUT));
		addInput(createInputCentered<gui::AHPort>(Vec(116.173, 173.561), module, Arp32::HOLD_INPUT));
		addInput(createInputCentered<gui::AHPort>(Vec(84.995, 173.705), module, Arp32::RANDOM_INPUT));
		addInput(createInputCentered<gui::AHPort>(Vec(36.028, 327.111), module, Arp32::PITCH_INPUT));
		addInput(createInputCentered<gui::AHPort>(Vec(103.309, 327.111), module, Arp32::CLOCK_INPUT));

		addOutput(createOutputCentered<gui::AHPort>(Vec(36.028, 228.311), module, Arp32::GATE_OUTPUT));
		addOutput(createOutputCentered<gui::AHPort>(Vec(103.309, 228.311), module, Arp32::EOC_OUTPUT));
		addOutput(createOutputCentered<gui::AHPort>(Vec(69.745, 278.811), module, Arp32::OUT_OUTPUT));

		if (module != NULL) {
			Arp32Display *displayW = createWidget<Arp32Display>(Vec(3, 115));
			displayW->box.size = Vec(100, 140);
			displayW->module = module;
			addChild(displayW);
		}

		gateOptions.emplace_back(std::string("Trigger"), Arp32::TRIGGER);
		gateOptions.emplace_back(std::string("Retrigger"), Arp32::RETRIGGER);
		gateOptions.emplace_back(std::string("Continuous"), Arp32::CONTINUOUS);

		noteOptions.emplace_back(std::string("Omit last note"), false);
		noteOptions.emplace_back(std::string("Play last note"), true);

	}

	void appendContextMenu(Menu *menu) override {

		Arp32 *arp = dynamic_cast<Arp32*>(module);
		assert(arp);

		struct Arp32Menu : MenuItem {
			Arp32 *module;
			Arp32Widget *parent;
		};

		struct GateModeItem : Arp32Menu {
			Arp32::GateMode gateMode;
			void onAction(const rack::widget::Widget::ActionEvent &e) override {
				module->gateMode = gateMode;
			}
		};

		struct GateModeMenu : Arp32Menu {
			Menu *createChildMenu() override {
				Menu *menu = new Menu;
				for (auto opt: parent->gateOptions) {
					GateModeItem *item = createMenuItem<GateModeItem>(opt.name, CHECKMARK(module->gateMode == opt.value));
					item->module = module;
					item->gateMode = opt.value;
					menu->addChild(item);
				}
				return menu;
			}
		};

		struct RepeatModeItem : Arp32Menu {
			bool repeatEnd;
			void onAction(const rack::widget::Widget::ActionEvent &e) override {
				module->repeatEnd = repeatEnd;
			}
		};

		struct RepeatModeMenu : Arp32Menu {
			Menu *createChildMenu() override {
				Menu *menu = new Menu;
				for (auto opt: parent->noteOptions) {
					RepeatModeItem *item = createMenuItem<RepeatModeItem>(opt.name, CHECKMARK(module->repeatEnd == opt.value));
					item->module = module;
					item->repeatEnd = opt.value;
					menu->addChild(item);
				}
				return menu;
			}
		};

		menu->addChild(construct<MenuLabel>());

		GateModeMenu *item = createMenuItem<GateModeMenu>("Gate Mode");
		item->module = arp;
		item->parent = this;
		menu->addChild(item);

		RepeatModeMenu *ritem = createMenuItem<RepeatModeMenu>("Play last note in cyclical patterns");
		ritem->module = arp;
		ritem->parent = this;
		menu->addChild(ritem);

	}

};

Model *modelArp32 = createModel<Arp32, Arp32Widget>("Arp32");

