This script will:

Connect to both modems (Brain on 3999/3998, Phoenix Nest on 5100/5101)
Generate TX for the same test message on both
Compare sample-by-sample - lengths, RMS, cross-correlation to find alignment lag
Extract symbols - downconvert to baseband, sample at symbol rate
Compare symbol-by-symbol - phase values, find first mismatch
Analyze preamble structure - look for D1/D2 patterns
Generate plots showing:

Waveform overlay
Zoomed preamble region
Power spectrum
Symbol constellation
Phase vs time
Phase difference



To run:
bash# Make sure both servers are running first
python compare_tx_output.py
What to look for:

Length difference → timing/frame structure issue
Phase difference starting immediately → preamble encoding different
Phase difference starting mid-preamble → D1/D2 position issue
Phase difference in data region only → interleaver/FEC encoding issue

The output will show exactly where the two modems diverge, which will point us to what needs fixing in Phoenix Nest TX.