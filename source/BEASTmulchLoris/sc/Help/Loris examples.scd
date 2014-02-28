s.boot;
f = RBE_SDIF_File("A.sdif".resolveRelative); // open a SDIF file with RBEP frames
p = BEPartials(f);
f.close;


(
SynthDef(\A, { arg stretch = 1, pitch = 1, bw = 1, mul = 1, gate = 1, attack=0, release=0.1, out=0;
var outmix, envs;

// returns [[freqEnvGen, a LorisPhaseGen, bandwidthEnvGen, ampEnvGen] * numPartials]
envs = p.ar(stretch, pitch, bw);

// pan each partial and mix
outmix = Mix(PanAz.ar(2, BEOsc.ar(*envs), {Rand(-1, 1)} ! p.size))
		* EnvGen.ar(Env.linen(attack, p.dur * stretch, release), gate, mul, doneAction: 2);

Out.ar(out, outmix);
}).load(s, dir: "".resolveRelative);
)

Synth(\A);
Synth(\A, [pitch: 0.5]);
Synth(\A, [stretch: 3]);
Synth(\A, [stretch: 3, bw:0]);
Synth(\A, [stretch: 3, bw:2]);




// inputs can be other UGens
// works well for stretch
(
SynthDef(\Astretch, { arg pitch = 1, bw = 1, mul = 1, gate = 1, attack=0, release=0.1, out=0;
var outmix, envs;

// normal onset, then hold....
envs = p.ar(EnvGen.kr(Env([1, 1, 10], [0.1, 0.05])), pitch, bw);

// pan each partial and mix
outmix = Mix(PanAz.ar(2, BEOsc.ar(*envs), {Rand(-1, 1)} ! p.size))
		* EnvGen.ar(Env.linen(attack, p.dur * 10, release), gate, mul, doneAction: 2);

Out.ar(out, outmix);
}).load(s, dir: "".resolveRelative);
)

Synth(\Astretch);
Synth(\Astretch, [pitch: 0.5]); // hello cello




// can use Functions for BEPartials:ar
// wobble partial pitch
(
SynthDef(\ApitchWobble, { arg stretch = 1, pitch = 1, bw = 1, mul = 1, gate = 1, attack=0, release=0.1, out=0;
var outmix, envs;

// randomly modulate pitch per partial
envs = p.ar(stretch, {SinOsc.kr(Rand(4.0, 10.0)).range(0.995, 1.005)}, bw);

// pan each partial and mix
outmix = Mix(PanAz.ar(2, BEOsc.ar(*envs), {Rand(-1, 1)} ! p.size))
		* EnvGen.ar(Env.linen(attack, p.dur * stretch, release), gate, mul, doneAction: 2);

Out.ar(out, outmix);
}).load(s, dir: "".resolveRelative);
)

Synth(\ApitchWobble, [stretch: 3]);




// rand pitch
(
SynthDef(\ApitchRand, { arg stretch = 1, pitch = 1, bw = 1, mul = 1, gate = 1, attack=0, release=0.1, out=0;
var outmix, envs;

// randomly scale pitch per partial with global scale
envs = p.ar(stretch, {Rand(1.0, 2.0) * pitch}, bw);

// pan each partial and mix
outmix = Mix(PanAz.ar(2, BEOsc.ar(*envs), {Rand(-1, 1)} ! p.size))
		* EnvGen.ar(Env.linen(attack, p.dur * stretch, release), gate, mul, doneAction: 2);

Out.ar(out, outmix);
}).load(s, dir: "".resolveRelative);
)

Synth(\ApitchRand, [stretch: 3]);
Synth(\ApitchRand, [stretch: 3]); // different every time
Synth(\ApitchRand, [stretch: 3, bw: 0]);
Synth(\ApitchRand, [stretch: 3, pitch: 0.5, bw: 5]); // exaggerate noise components



////// manipulate partialList directly

// sort by frequency and then only odd numbered partials (numbers start at 0, so use 'even')
q = p.copy;
q.partialList = q.partialList.sort({|a, b| a.freqs.mean <= b.freqs.mean }).select({|item, i| i.even });

(
SynthDef(\Aodd, { arg stretch = 1, pitch = 1, bw = 1, mul = 1, gate = 1, attack=0, release=0.1, out=0;
var outmix, envs;

// returns [[freqEnvGen, a LorisPhaseGen, bandwidthEnvGen, ampEnvGen] * numPartials]
envs = q.ar(stretch, pitch, bw);

// pan each partial and mix
outmix = Mix(PanAz.ar(2, BEOsc.ar(*envs), {Rand(-1, 1)} ! q.size))
		* EnvGen.ar(Env.linen(attack, q.dur * stretch, release), gate, mul, doneAction: 2);

Out.ar(out, outmix);
}).load(s, dir: "".resolveRelative);
)

Synth(\Aodd);




// thin partialList by keeping only loud partials
r = p.copy;
r.partialList = r.partialList.select({|part| part.amps.maxItem > 0.005 });

(
SynthDef(\Athin, { arg stretch = 1, pitch = 1, bw = 1, mul = 1, gate = 1, attack=0, release=0.1, out=0;
var outmix, envs;

// returns [[freqEnvGen, a LorisPhaseGen, bandwidthEnvGen, ampEnvGen] * numPartials]
envs = r.ar(stretch, pitch, bw);

// pan each partial and mix
outmix = Mix(PanAz.ar(2, BEOsc.ar(*envs), {Rand(-1, 1)} ! r.size))
		* EnvGen.ar(Env.linen(attack, r.dur * stretch, release), gate, mul, doneAction: 2);

Out.ar(out, outmix);
}).load(s, dir: "".resolveRelative);
)

Synth(\Athin); // reasonable quality sound but cheaper in CPU



// thin partialList by keeping only quiet partials
t = p.copy;
t.partialList = t.partialList.select({|part| part.amps.maxItem < 0.005 });

(
SynthDef(\Aquiet, { arg stretch = 1, pitch = 1, bw = 1, mul = 1, gate = 1, attack=0, release=0.1, out=0;
var outmix, envs;

// returns [[freqEnvGen, a LorisPhaseGen, bandwidthEnvGen, ampEnvGen] * numPartials]
envs = t.ar(stretch, pitch, bw);

// pan each partial and mix
outmix = Mix(PanAz.ar(2, BEOsc.ar(*envs), {Rand(-1, 1)} ! t.size))
		* EnvGen.ar(Env.linen(attack, t.dur * stretch, release), gate, mul, doneAction: 2);

Out.ar(out, outmix);
}).load(s, dir: "".resolveRelative);
)

Synth(\Aquiet);
Synth(\Aquiet, [stretch: 6, pitch: 0.125, bw: 2, mul: 6]);