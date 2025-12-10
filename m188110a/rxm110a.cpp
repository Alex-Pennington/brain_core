/*
 *
 * MIL-STD 188-110A Serial tone modem.
 *
 *
 */
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "Cm110s.h"
// Qt removed: #include <QDebug>

//#include <direct.h>  //file i/o test data saving not needed when testing done

extern float rx_coffs[M1_RX_FILTER_LENGTH];
extern int   p_c_seq[9];


// Constellation routines
//
// Save to the constellation queue
//
void Cm110s::rx_update_constellation( FComplex sym )
{
	//if( m_c_q.size() < 500 ) m_c_q.push( sym );
	if( m_c_q.size() < 2400 ) m_c_q.push( sym );
}
void Cm110s::rx_update_pos_eq_constellation( FComplex sym )
{

	//if( m_c_q.size() < 500 ) m_c_q.push( sym );
	if( m_c_q_peq.size() < 2400 ) m_c_q_peq.push( sym );

}

void Cm110s::training_dcd_reset( void )
{
	m_train_count = m_train_errors = 0;
}
void Cm110s::training_dcd_decision( void )
{
	float f1,f2,min,max;
	
	/*
	char text[80];
	sprintf_s(text, "m_train_errors = %3.0f\r\n", float(m_train_errors));
	func_status(TEXT_STATUS, text);
	*/

	if((rx_mode == M75NS ) || ( rx_mode == M75NL))
	{
		
		min = max = sync_75_mask[0];
		for( int i=0; i < SYNC_75_MASK_LENGTH; i++ )
		{
			if( sync_75_mask[i] < min ) min = sync_75_mask[i];
			if( sync_75_mask[i] > max ) max = sync_75_mask[i];
		}

		/*
		sprintf_s(text, "75bps min = %f\r\n", min);
		func_status(TEXT_STATUS, text);

		sprintf_s(text, "75bps max = %f\r\n", max);
		func_status(TEXT_STATUS, text);
		*/

		//if(( max/min) < .001 ) rx_reset();  // was 2 by Charles
		//if(( max/min) < 2 && viterbi_state == 0 ) rx_reset(); 


		// viterbi_state always 0 for 75bps
		//if(( max/min) < 2 && viterbi_state == 0 )

		if ((max / min) < 20 && viterbi_state == 0)
		{	

			viterbi_state = 1;

            static char ts[50];
        //	time_t rawtime;
        //	rawtime = time(NULL);
        //	struct tm timeinfo;
        //	gmtime_s(&timeinfo, &rawtime);
        //	strftime(ts,50,"%b-%d-%Y %X",&timeinfo);

			char text[80];

			//float value = max/min;
			//sprintf_s(text,"%s: max/min = %3.2f\r\n",ts,value);
			//func_status(TEXT_STATUS,text);

        //	if (isnan(datasnr) || isinf(datasnr))
        //	{
        //		float dummysnr = 0.0;
        //        sprintf(text, "%s: DCD DECISION SNR: (%3.0f db)\r\n", ts, dummysnr);
        //		func_status(TEXT_STATUS, text);
        //	}
        //	else
        //	{
         //       sprintf(text, "%s: DCD DECISION SNR: (%3.0f db)\r\n", ts, datasnr);
        //		func_status(TEXT_STATUS, text);
        //	}


        //	sprintf_s(text,"%s: DCD DECISION/VITERBI STATE RESET REQUEST\r\n",ts);
        //	func_status(TEXT_STATUS,text);

			rx_reset(); 
		}


/*
		// introduced in v1.01 b1.0.1.4 TB#6.6.1
		if(pmag_sql_factor>=9)
		{
			if(( max/min) < 2 ) rx_reset();  // SQL ON HI or LO
		}
		else
		{
			if(( max/min) < .001 ) rx_reset(); // SQL OFF
		}
*/





		/*
		float maxmin;
		maxmin = max/min;
		FILE *out_fp; // File pointer of logging file
		static char outFileName[255];
		static char *CurrentPath;
		static char *outlogSubDirectory;
		int curdrive;

		outlogSubDirectory = "\\outlog\\";

		// Fix to allow for support on any system drive
		curdrive = _getdrive();
		CurrentPath = _getdcwd( curdrive, NULL, 0 );

		strcpy(outFileName,CurrentPath);
		strcat(outFileName,outlogSubDirectory);
		strcat(outFileName,"test75bps");
		strcat(outFileName,".out");

		//MessageBox (NULL, outFileName, "test.out Path", MB_OK);

		out_fp = fopen(outFileName,"a");

		if(out_fp != NULL)
		{
			//fputs(LPCTSTR(out),out_fp);
			fprintf (out_fp,"%f\n",f1);

		}
		fclose(out_fp);
		*/

	}
	else
	{
		f1 = (float)m_train_count;
		f2 = (float)m_train_errors;
		//if( ( f2/f1) > 0.2 ) rx_reset();  // was 0.2 or 0.3 by Charles
		//if( ( f2/f1) > 0.12 ) rx_reset();  // was 0.2 or 0.3 by Charles
		//if( ( f2/f1) > 0.05 ) rx_reset();  // Try this		

		// This resets usually before garbage if no EOM at < 400 BER reading.
		//if( ( f2/f1) > 0.15 ) rx_reset();  // Best when used
		//training_dcd_reset();

/*
			static char ts[50];
			time_t rawtime;
			rawtime = time(NULL);
			struct tm timeinfo;
			gmtime_s(&timeinfo, &rawtime);
			strftime(ts,50,"%b-%d-%Y %X",&timeinfo);

			char text[80];

			float value = f2/f1;
			sprintf_s(text,"%s: f2/f1 = %3.2f\r\n",ts,value);
			func_status(TEXT_STATUS,text);

			sprintf_s(text," viterbi_state = %i", viterbi_state);
			func_status(TEXT_STATUS,text);
*/

// 0.15 works fine but takes a bit too long
//if( ( f2/f1) > 0.15 && viterbi_state == 0 ) 

// 0.05 .. 0.10 resets before EOM on bad channel compared to H build
//if( ( f2/f1) > 0.10 && viterbi_state == 0 )

		//if (((f2 / f1) > 0.30 && viterbi_state == 0) || (m_train_errors >= 650 && viterbi_state == 0))

		if ((f2 / f1) > 0.30 && viterbi_state == 0)
		{			

			viterbi_state = 1;

    //		static char ts[50];
    //		time_t rawtime;
    //		rawtime = time(NULL);
    //		struct tm timeinfo;
    //		gmtime_s(&timeinfo, &rawtime);
    //		strftime(ts,50,"%b-%d-%Y %X",&timeinfo);

    //		char text[80];
			
			/*
			float value = f2/f1;
			sprintf_s(text,"%s: f2/f1 = %f\r\n",ts,value);
			func_status(TEXT_STATUS,text);
			*/

    //		if (isnan(datasnr) || isinf(datasnr))
    //		{
    //			float dummysnr = 0.0;
    //			sprintf_s(text, "%s: DCD DECISION SNR: (%3.0f db)\r\n", ts, dummysnr);
    //			func_status(TEXT_STATUS, text);
    //		}
    //		else
    //		{
    //			sprintf_s(text, "%s: DCD DECISION SNR: (%3.0f db)\r\n", ts, datasnr);
    //			func_status(TEXT_STATUS, text);
    //		}




    //		sprintf_s(text,"%s: DCD DECISION/VITERBI STATE RESET REQUEST\r\n",ts);
    //		func_status(TEXT_STATUS,text);

			rx_reset();
		}


	training_dcd_reset();


	//Right now for 150bps and up we do nothing		

/*		
		FILE *out_fp; // File pointer of logging file
		static char outFileName[255];
		static char *CurrentPath;
		static char *outlogSubDirectory;
		int curdrive;

		outlogSubDirectory = "\\outlog\\";

		// Fix to allow for support on any system drive
		curdrive = _getdrive();
		CurrentPath = _getdcwd( curdrive, NULL, 0 );

		strcpy(outFileName,CurrentPath);
		strcat(outFileName,outlogSubDirectory);
		strcat(outFileName,"F1F2test");
		strcat(outFileName,".out");

		//MessageBox (NULL, outFileName, "test.out Path", MB_OK);

		out_fp = fopen(outFileName,"a");

		if(out_fp != NULL)
		{
			//fputs(LPCTSTR(out),out_fp);
			fprintf (out_fp,"%f\n",f1);

		}
		fclose(out_fp);

		strcpy(outFileName,"");

		outlogSubDirectory = "\\outlog\\";

		// Fix to allow for support on any system drive
		curdrive = _getdrive();
		CurrentPath = _getdcwd( curdrive, NULL, 0 );

		strcpy(outFileName,CurrentPath);
		strcat(outFileName,outlogSubDirectory);
		strcat(outFileName,"F2F1test");
		strcat(outFileName,".out");

		//MessageBox (NULL, outFileName, "test.out Path", MB_OK);

		out_fp = fopen(outFileName,"a");

		if(out_fp != NULL)
		{
			//fputs(LPCTSTR(out),out_fp);
			fprintf (out_fp,"%f\n",f2);

		}
		fclose(out_fp);
*/

	}
}
float Cm110s::twos_to_float( unsigned short in )
{
	float val;
	
	if(in&0x8000)
    {
       	val = -(float)(((~in)&0x7FFF)*0.000030517578125);
    }
    else
    {
        val = (float)(in*0.000030517578125);
    }
    return(val);
}
void Cm110s::new_eom_rx_reset(void)
{

//	static char ts[50];
//	time_t rawtime;
//	rawtime = time(NULL);
//	struct tm timeinfo;
//	gmtime_s(&timeinfo, &rawtime);
//	strftime(ts, 50, "%b-%d-%Y %X", &timeinfo);

//	char text[80];

//	if (isnan(datasnr) || isinf(datasnr))
//	{
//		float dummysnr = 0.0;
//		sprintf_s(text, "%s: Core EOM Detected SNR: (%3.0f db)\r\n", ts, dummysnr);
//		func_status(TEXT_STATUS, text);
//	}
//	else
//	{
//		sprintf_s(text, "%s: Core EOM Detected SNR: (%3.0f db)\r\n", ts, datasnr);
//		func_status(TEXT_STATUS, text);
//	}
//
//
//	sprintf_s(text, "%s: Core EOM Detected Modem Reset\r\n", ts);
//	func_status(TEXT_STATUS, text);

	//Added as a test
	//frame_start_position = 0;


	/*
	init_preamble_module();
	create_data_scrambler_seq();
	create_prx_tables();
	create_mes_and_mns_seq();
	viterbi_init();
	equalize_init();
	//rx_callbk   = NULL;
	m_hold      = 1.0;
	kalman_init();
	demodulate_reset( rx_mode );
	training_dcd_reset();
	*/
	//rx_mode = M300L;										//BB October 2022 Was over-riding the MATCHDR setting in stub functions.

	func_status(DCD_FALSE_STATUS, NULL);
	rx_state = RX_HUNTING_PREAMBLE_STATE;
    sprintf(m_state_text, "IDLE");
	rx_known_errors = 0;
	viterbi_decode_reset();
	state_text();

	//EOM_COUNT_RESET
	float eom_count_reset;
	eom_count_reset = 9999.0;
	func_status(TEXT_STATUS, &eom_count_reset);


}
void Cm110s::eom_rx_reset(void)
{
//    qDebug()<<Q_FUNC_INFO <<"<Entered";
//	static char ts[50];
//	time_t rawtime;
//	rawtime = time(NULL);
//	struct tm timeinfo;
//	gmtime_s(&timeinfo, &rawtime);
//	strftime(ts, 50, "%b-%d-%Y %X", &timeinfo);
//
//	char text[80];
//
//	if (isnan(datasnr) || isinf(datasnr))
//	{
//		float dummysnr = 0.0;
//		sprintf_s(text, "%s: EOM Detected SNR: (%3.0f db)\r\n", ts, dummysnr);
//		func_status(TEXT_STATUS, text);
//	}
//	else
//	{
//		sprintf_s(text, "%s: EOM Detected SNR: (%3.0f db)\r\n", ts, datasnr);
//		func_status(TEXT_STATUS, text);
//	}
//
//
//	sprintf_s(text,"%s: EOM Detected Modem Reset\r\n",ts);
//	func_status(TEXT_STATUS, text );

	//Added as a test
	//frame_start_position = 0;


/*
	init_preamble_module();	
	create_data_scrambler_seq();
	create_prx_tables();
	create_mes_and_mns_seq();
	viterbi_init();
	equalize_init();
	//rx_callbk   = NULL;
	m_hold      = 1.0;
	kalman_init();
	demodulate_reset( rx_mode );
	training_dcd_reset();
*/

	//rx_mode = M300L;														//BB October 2022 Was over-riding the MATCHDR setting in stub functions.

	func_status( DCD_FALSE_STATUS, NULL );
	rx_state        = RX_HUNTING_PREAMBLE_STATE;
    sprintf( m_state_text,"IDLE");
	rx_known_errors = 0;
	viterbi_decode_reset();
	state_text();

	//EOM_COUNT_RESET
	float eom_count_reset;
	eom_count_reset = 9999.0;
	func_status(TEXT_STATUS, &eom_count_reset );


}
void Cm110s::post_eom_rx_reset( void )
{

//	static char ts[50];
//	time_t rawtime;
//	rawtime = time(NULL);
//	struct tm timeinfo;
//	gmtime_s(&timeinfo, &rawtime);
//	strftime(ts,50,"%b-%d-%Y %X",&timeinfo);
//
//	char text[80];
//
//	if (isnan(datasnr) || isinf(datasnr))
//	{
//		float dummysnr = 0.0;
//		sprintf_s(text, "%s: Post EOM Detected SNR (%3.0f db)\r\n", ts, dummysnr);
//		func_status(TEXT_STATUS, text);
//	}
//	else
//	{
//		sprintf_s(text, "%s: Post EOM Detected SNR (%3.0f db)\r\n", ts, datasnr);
//		func_status(TEXT_STATUS, text);
//	}
//
//	sprintf_s(text,"%s: Post EOM Modem Reset\r\n",ts);
//	func_status(TEXT_STATUS, text );

	//Added as a test
	//frame_start_position = 0;


/*
	init_preamble_module();	
	create_data_scrambler_seq();
	create_prx_tables();
	create_mes_and_mns_seq();
	viterbi_init();
	equalize_init();
	//rx_callbk   = NULL;
	m_hold      = 1.0;
	kalman_init();
	demodulate_reset( rx_mode );
	training_dcd_reset();
*/

	//rx_mode = M300L;                            //BB October 2022.  Was overriding MATCHDR setting in stub functions.

	func_status( DCD_FALSE_STATUS, NULL );
	rx_state        = RX_HUNTING_PREAMBLE_STATE;
    sprintf( m_state_text,"IDLE");
	rx_known_errors = 0;
	viterbi_decode_reset();
	state_text();

}
void Cm110s::rx_reset( void )
{

//	static char ts[50];
//	time_t rawtime;
//	rawtime = time(NULL);
//	struct tm timeinfo;
//	gmtime_s(&timeinfo, &rawtime);
//	strftime(ts,50,"%b-%d-%Y %X",&timeinfo);
//
//	char text[80];
//	sprintf_s(text,"%s: Modem Reset\r\n",ts);
//	func_status(TEXT_STATUS, text );

	//Added as a test
	//frame_start_position = 0;


/*
	init_preamble_module();	
	create_data_scrambler_seq();
	create_prx_tables();
	create_mes_and_mns_seq();
	viterbi_init();
	equalize_init();
	//rx_callbk   = NULL;
	m_hold      = 1.0;
	kalman_init();
	demodulate_reset( rx_mode );
	training_dcd_reset();
*/

	//rx_mode = M300L;										//BB October 2022.  Was overriding MATCHDR setting in stub functions.

	
	func_status( DCD_FALSE_STATUS, NULL );
	rx_state        = RX_HUNTING_PREAMBLE_STATE;
    sprintf( m_state_text,"IDLE");
	rx_known_errors = 0;
	viterbi_decode_reset();
	state_text();

	//EOM_COUNT_RESET
	float eom_count_reset;
	eom_count_reset = 9999.0;
	func_status(TEXT_STATUS, &eom_count_reset );

}
void Cm110s::reset_frame_start( int val )
{
	frame_start_position = val-M1_FF_EQ_LENGTH/2;

//	static char ts[50];
//	time_t rawtime;
//	rawtime = time(NULL);
//	struct tm timeinfo;
//	gmtime_s(&timeinfo, &rawtime);
//
//	char text[80];
//
//	strftime(ts,50,"%b-%d-%Y %X",&timeinfo);
//	sprintf_s(text,"%s: Reset Frame Restart = %i\r\n",ts,frame_start_position );
//	func_status(TEXT_STATUS,text);

}
int Cm110s::get_frame_start( void )
{
	return frame_start_position;
}
/*
 *
 * After we have detected a preamble use this routine to correct
 * for the fine frequency error. It must be called on every received 
 * block.
 *
 */
void Cm110s::remove_frequency_error( FComplex *in, float delta, int length )
{
	int i;
	static float acc;
	FComplex osc,temp;

	for( i = 0; i < length ; i++ )
	{
		/* Update with new samples */
		osc.re =    (float)cos(acc);
		osc.im =   -(float)sin(acc);
		acc   -=  delta;

		if( acc >= (float)( 2*PI)) acc -= (float)(2*PI);			
		if( acc <= (float)(-2*PI)) acc += (float)(2*PI);			
		
		temp.re = cmultReal(in[i],osc);
		temp.im = cmultImag(in[i],osc);

		in[i] = temp;

/*
		// Interesting affect of displaying a rotating
		// group of symbols center of the Constellation
		// display indicating how much frequency correction is
		// taking place.

		// Constellation Symbols
		FComplex    consym;
		consym.re = temp.re *10; //Gain: 50..150
		consym.im = temp.im *10; //Gain: 50..150

		// Constellation Routines
		rx_update_constellation( consym );
*/

	}
}
void Cm110s::translate_seq_in_freq( FComplex *in, FComplex *out, float delta, int length )
{
	int i;
	float acc;
	FComplex osc;
	
	acc = 0;
	
	for( i = 0; i < length ; i++ )
	{
		/* Update with new samples */
		osc.re =    (float)cos(acc);
		osc.im =   -(float)sin(acc);




	// Alwayes returns 2400 symbols while Hunting Preamble
	
	// Displays a perfect circle on the Constellation Display
	
	// if osc.re *1, needs to be greater to hide
		
	// Constellation Symbols
	FComplex    consym;
	consym.re = osc.re *1; //Gain: 50..150
	consym.im = osc.im *1; //Gain: 50..150

	// Constellation Routines
	rx_update_constellation( consym );




		out[i].re = cmultReal(in[i],osc);
		out[i].im = cmultImag(in[i],osc);
		acc  -=  delta;

		if( acc >= (float)( 2*PI)) acc -= (float)(2*PI);			
		if( acc <= (float)(-2*PI)) acc += (float)(2*PI);	

	}
}
FComplex Cm110s::snr( FComplex in  )
{
	float snrmag;
	float snr;

	snrmag = (in.re*in.re)+(in.im*in.im);

	//Update SNR for display in db
	snr = (float) 20.0*(log10(snrmag));
	//snr = (float) 10.0*(log10(snrmag));
	snr = (snr+lastsnr)/2;
	if (isnan(snr) || isinf(snr))
	{
		func_status(SNR_STATUS, &lastsnr);
	}
	else
	{
		func_status(SNR_STATUS, &snr);
	}
	lastsnr = snr;

	return in;
}
FComplex Cm110s::agc( FComplex in  )
{

	float        h,mag;
	
	mag = (in.re*in.re)+(in.im*in.im);

	h = (float)((M1_LAMBDA*mag) + (1.0-M1_LAMBDA)*m_hold);

	m_hold = h;		

	// Gain
	h     = (float)(0.045/sqrt(h)); // works good makes nice AGC-1

	//h     = (float)(0.035/sqrt(h)); // works good as AGC always ON


	//h     = (float)(0.02/sqrt(h)); // works good
	//h     = (float)(1.0/sqrt(h)); //Charles


	in.re = (in.re*h);
	in.im = (in.im*h);

	//TEST
	//char text[80];
	//sprintf_s(text,"***** AGC Hold (%3.3f)\r\n",h );
	//func_status(TEXT_STATUS,text);

	return in;

}
FComplex Cm110s::agc_2( FComplex in  )
{
	float        h,mag;
	
	mag = (in.re*in.re)+(in.im*in.im);

	h = (float)((M1_LAMBDA*mag) + (1.0-M1_LAMBDA)*m_hold);

	m_hold = h;		

	// Gain

	h     = (float)(0.1/sqrt(h)); // works good

	//h     = (float)(1.0/sqrt(h)); //Charles


	in.re = (in.re*h);
	in.im = (in.im*h);

	//TEST
	//char text[80];
	//sprintf_s(text,"***** AGC Hold (%3.3f)\r\n",h );
	//func_status(TEXT_STATUS,text);

	return in;

}
void Cm110s::set_rx_mode( Mode mode )
{
	rx_mode = mode;
	set_de_interleave_mode( rx_mode, &deblk[0] );
	set_de_interleave_mode( rx_mode, &deblk[1] );
	demodulate_reset( rx_mode );
}
//
// The magnitude of the accumulated product of the conjugate of two signals.
//
float Cm110s::match_sequence( FComplex *in, FComplex *seq, int length )
{
	FComplex temp;

	temp.re = temp.im = 0;

	for( int i = 0; i < length; i++ )
	{
		temp.re += cmultRealConj(in[i*2],seq[i]);
		temp.im += cmultImagConj(in[i*2],seq[i]);
	}
	return( temp.re*temp.re + temp.im*temp.im );
}
FComplex Cm110s::filter( FComplex *in, const float *coffs, int length )
{
	FComplex out;
	int i;
	
	out.re = in[0].re*coffs[0];
	out.im = in[0].im*coffs[0];
	
	for( i = 1; i < length; i++ )
	{
		out.re += in[i].re*coffs[i];
		out.im += in[i].im*coffs[i];	
	}



/*
	// Interesting but unless here

	// Constellation Symbols
	FComplex    consym;
	consym.re = out.re *150; //Gain: 50..150
	consym.im = out.im *150; //Gain: 50..150

	// Constellation Routines
	rx_update_constellation( consym );
*/

	return out;
}



Mode Cm110s::find_mode_value(FComplex *in)
{
	float    mag, max = 0;
	int      i;
	Mode     mode = M75NS;

	for (i = M75NS; i <= M4800S; i++)
	{
		mag = match_sequence(in, p_mode[i], M1_P_MODE_LENGTH);
		if (mag > max)
		{
			max = mag;
			mode = (Mode)i;
		}
	}

	// 

	// MELPe test
	if (mode == 13)
	{
		//600 Harris Voice
		mode = (Mode)12;
	}
	if (mode == 15)
	{
		//1200 Harris Voice
		mode = (Mode)14;
	}

	if ((mode == 12) || (mode == 14) || (mode == 16))
	{
		//Automatic SYNC mode selection in support of Digital Voice
		float sync_mode_reset;
		sync_mode_reset = 7777.0;
		func_status(TEXT_STATUS, &sync_mode_reset);
	}

	return mode;
}
bool Cm110s::verify_mode( Mode mode, int count )
{
	if( count > 2 )
	{
		// Must be long Interleave
		if( M75NS == mode ) return false;   
		if( M150S == mode ) return false;   
		if( M300S == mode ) return false;   
		if( M600S == mode ) return false;
		if( M600V == mode ) return false;
		if( M1200S == mode ) return false;  
		if( M1200V == mode ) return false;
		if( M2400S == mode ) return false;  
		if( M2400V == mode ) return false;  
		if( M4800S == mode ) return false;  
	}
	return true;
}
int Cm110s::find_count_value( FComplex *in )
{
	float   mag,max=0;
	int     i,count=0; 
	
	for( i = 0; i < 24; i++ )
	{
		mag = match_sequence( in, p_count[i], M1_P_COUNT_LENGTH );
		if( mag > max )
		{
			max = mag;
			count = i;
		}
	}

/*
			static char ts[50];
			time_t rawtime;
			rawtime = time(NULL);
			struct tm timeinfo;
			gmtime_s(&timeinfo, &rawtime);
			strftime(ts,50,"%b-%d-%Y %X",&timeinfo);

			char text[80];
			sprintf_s(text,"%s: newPREAM%i\r\n",ts,count);
			// Send to MS-DMT Scrolling Status Window 
			func_status(TEXT_STATUS,text);
*/

	return count;
}
/*
 * Broken up into shorter sequencies to improve 
 * off frequency aquisition.
 *
 */
float Cm110s::correlate_common_preamble( FComplex *in )
{
	float mag;
	int i;
	
	mag = 0;
		
	for( i = 0; i < 9; i++ )
	{
		mag += match_sequence( &in[i*32*2], &p_common[i*32], 32 );
	}

	return(mag);
}
/*
 * By shortening the preamble sequence we are able to detect a greater 
 * frequency offset.
 *
 * The incoming signal is matched against 3 channels spaced 50 Hz apart.
 *
 * Then by looking at the repeated output from the correlator we will be able 
 * to measure the frequency error seen in the preamble.
 *
 */
float Cm110s::calculate_frequency_error( FComplex *in )
{
	FComplex temp[192];
	float real;
	float imag;
	int i;
	
	/* First decramble the sequence */
	temp[0].re = cmultRealConj(in[0],p_common[0]);
	temp[0].im = cmultImagConj(in[0],p_common[0]);	

	for( i = 1 ; i < 192 ; i++ )
	{
		temp[i].re = cmultRealConj(in[i*2],p_common[i]);	
		temp[i].im = cmultImagConj(in[i*2],p_common[i]);	
	}


	/* Now calculate the fine frequency error */

	real = cmultRealConj(temp[0],temp[32]);	
	imag = cmultImagConj(temp[0],temp[32]);	

	for( i = 1 ; i < 160 ; i++ )
	{
		real += cmultRealConj(temp[i],temp[i+32]);	
		imag += cmultImagConj(temp[i],temp[i+32]);	
	}

	real   = (float)(atan2(imag,real)/64);

	return(real);
} 
int Cm110s::extract_preamble_symbol( FComplex *in )
{
	int i,max_int;
	float max,mag;

	max     = 0;
	max_int = 0;

	for( i = 0; i < 8; i++ )
	{
		mag = match_sequence( in, prx[i], 32 );
		if( mag > max )
		{
			max     = mag;
			max_int = i;
		}
	}
	return max_int;
}
bool Cm110s::verify_preamble( FComplex *in )
{

	return true;


	/*
	
	// H build test
	for( int i = 0; i < 9; i++ )
	{

		char text[80];

		sprintf_s(text,"Extract Preamble Symbol 1 %i/%i\r\n",i,extract_preamble_symbol( &in[i*64] ));		
		func_status(TEXT_STATUS,text);	

		sprintf_s(text,"Extract Preamble Symbol 2 %i/%i\r\n",i,p_c_seq[i]);
		func_status(TEXT_STATUS,text);	


		//if( p_c_seq[i] != extract_preamble_symbol( &in[i*64] )) return false; 

	}
	return true;
	*/

	/*
	return true;

	// Charles had this bypassed

	// p_c_seq[0] ... p_c_seq[8]= 0,1,3,0,1,3,1,2,0

	//return true;

	for( int i = 1; i <=9; i++ ) //9
	{
		//if( p_c_seq[i] != extract_preamble_symbol( &in[i*64] )) return false; 

		//extract_preamble_symbol( &in[i*64] )

		char text[80];
		sprintf_s(text,"Extract Preamble Symbol %i/%i\r\n",i,extract_preamble_symbol( &in[i*32*2] ));
		func_status(TEXT_STATUS,text);

	}
	return true;
	*/

}
int Cm110s::new_start_find_frequency_error_and_start_of_preamble( FComplex *in, float &error, float &pmag, Mode &mode, int &count )
{

	FComplex hi[M1_RX_BLOCK_LENGTH];
	FComplex lo[M1_RX_BLOCK_LENGTH];
	FComplex tp[M1_RX_BLOCK_LENGTH];

	float    mag;
	float    min;
	float    f_offset;
	int      i,start,index;
	float    on_channel_max=0;
	float    lo_channel_max=0;
	float    hi_channel_max=0;
	float    on_channel_min=100000;
	float    lo_channel_min=100000;
	float    hi_channel_min=100000;

	int      on_channel_pos=0;
	int      lo_channel_pos=0;
	int      hi_channel_pos=0;
	int      block_length;


	//BIG BOY
//	if(nr_state == 0)
//	{
		f_offset = (float)((50.0*PI)/(2400.0)); // Charles
//	}
//	else
//	{
//		f_offset = (float)((10.0*PI)/(2400.0));
//	}


	// Take into acount the length of the equaliser 
	index = M1_FF_EQ_LENGTH/2;
	// Input has been decimated by 2
	block_length = M1_RX_BLOCK_LENGTH/2;
	
    /* Do on frequency channel */
        
	for( i=0; i < block_length; i++, index++ )
	{
		mag = correlate_common_preamble( &in[index] );

		m_p_mag[ON_CHANNEL][0][index] = m_p_mag[ON_CHANNEL][1][index];
		m_p_mag[ON_CHANNEL][1][index] = mag;

		// Acumulate the two mags
		mag += m_p_mag[ON_CHANNEL][0][index];

		if( mag > on_channel_max ) 
		{
			on_channel_max    = mag;
			on_channel_pos    = i;
		}
		if( mag < on_channel_min )
		{
			on_channel_min = mag;
		}
	}

	/* Generate a hi channel */
	index = M1_FF_EQ_LENGTH/2;	
	translate_seq_in_freq( in, hi,  f_offset, M1_RX_BLOCK_LENGTH );

	for( i=0; i < block_length; i++, index++ )
	{
		mag = correlate_common_preamble( &hi[index] );

		m_p_mag[HI_CHANNEL][0][index] = m_p_mag[HI_CHANNEL][1][index];
		m_p_mag[HI_CHANNEL][1][index] = mag;

		// Acumulate the two mags
		mag += m_p_mag[HI_CHANNEL][0][index];

		if( mag > hi_channel_max ) 
		{
			hi_channel_max    = mag;
			hi_channel_pos    = i;
		}
		if( mag < hi_channel_min )
		{
			hi_channel_min = mag;
		}
	}

	/* Generate lo channel */
	index = M1_FF_EQ_LENGTH/2;
	translate_seq_in_freq( in, lo, -f_offset, M1_RX_BLOCK_LENGTH );
	
	for( i=0; i < block_length; i++, index++ )
	{
		mag = correlate_common_preamble( &lo[index] );

		m_p_mag[LO_CHANNEL][0][index] = m_p_mag[LO_CHANNEL][1][index];
		m_p_mag[LO_CHANNEL][1][index] = mag;

		// Acumulate the two mags
		mag += m_p_mag[LO_CHANNEL][0][index];

		if( mag > lo_channel_max ) 
		{
			lo_channel_max    = mag;
			lo_channel_pos    = i;
		}
		if( mag < lo_channel_min )
		{
			lo_channel_min = mag;
		}
	}

	if( (on_channel_max > lo_channel_max) && ( on_channel_max > hi_channel_max))
	{
		start   = on_channel_pos+M1_FF_EQ_LENGTH/2;
		error   = calculate_frequency_error( &in[start] );
		pmag    = on_channel_max;
		min     = on_channel_min;
		max_p_channel = ON_CHANNEL;
	}
	else
	{
		if(( hi_channel_max > lo_channel_max ) && ( hi_channel_max > on_channel_max))
		{
			start   = hi_channel_pos+M1_FF_EQ_LENGTH/2;
			error   = calculate_frequency_error( &hi[start] )+f_offset;
			pmag    = hi_channel_max;
			min     = hi_channel_min;
			max_p_channel = HI_CHANNEL;
		}
		else
		{
			start   = lo_channel_pos+M1_FF_EQ_LENGTH/2;
			error   = calculate_frequency_error( &lo[start] )-f_offset;
			pmag    = lo_channel_max;
			min     = lo_channel_min;
			max_p_channel = LO_CHANNEL;
		}		
	}
	



	// ADDED TEST
	if( tx_state == TX_IDLE_STATE )
	{
		float d_error;
		static float  delta_error;
		delta_error            = error;
		d_error        = (float)(-delta_error*2400.0/PI);
		func_status( FREQ_ERROR_STATUS, &d_error );
		lastderror = d_error;

	}


	// During preamble hunt
	//Update SNR for display in db
	float snr;
	snr = (float) 20.0*(log10(pmag*2));
	//snr = (float) 10.0*(log10(pmag));
	//snr = (float) 10.0*(log10(pmag/2));
	snr = ((snr+lasthuntsnr)/2.5f)-1.5f; //2
	if (isnan(snr) || isinf(snr))
	{
		func_status(SNR_STATUS, &lasthuntsnr);
	}
	else
	{
		func_status(SNR_STATUS, &snr); //HUNT
	}
	lastsnr = snr;
	lasthuntsnr = snr;

	//if( pmag > 10.0*min ) < Charles
	// Test in SDT v1.0.1 BUILD 1.0.1.0
	//if( pmag > 5.0*min ) < was using this in v1.01 b1.0.1.1. and v1.01 b1.0.1.2. but lots of falsing
	//if( pmag > 8.0*min ) < used this until PMAG Squelch introduced in v1.01 b1.0.1.4 TB#6.6

	//if( pmag > pmag_sql_factor*min )


	if( pmag > 0.50 )
	{


		// During preamble decoding
		//Update SNR for display in db
		float snr;
		//snr = (float) 20.0*(log10(pmag));
		snr = (float) 10.0*(log10(pmag/2));
		//snr = (snr+lastpreamblesnr)/2;
		if (isnan(snr) || isinf(snr))
		{
			func_status(SNR_STATUS, &lastsnr);
		}
		else
		{
			func_status(SNR_STATUS, &snr);
		}
		lastsnr = snr;
		if (isnan(snr) || isinf(snr))
		{
			func_status(SNR_STATUS, &lastpreamblesnr);
		}
		else
		{
			func_status(PREAM_SNR_STATUS, &snr); // PREAMBLE
		}
		preamsnr = snr;
		lastpreamblesnr = snr;

		/* Correct the information bearing section */
		translate_seq_in_freq( &in[start+(M1_P_COMMON_LENGTH*2)], tp, error, (M1_P_MODE_LENGTH+M1_P_COUNT_LENGTH)*2 );

		// ADDED TEST
		if( tx_state == TX_IDLE_STATE )
		{		
			float d_error;
			static float  delta_error;
			delta_error            = error;
			d_error        = (float)(-delta_error*2400.0/PI);
			func_status( FREQ_ERROR_STATUS, &d_error );
			lastderror = d_error;
		}

		/* Find mode parameters */
		mode   = find_mode_value( tp );
		/* Find count parameters */
		count  = find_count_value( &tp[M1_P_MODE_LENGTH*2] );		

		if( verify_mode( mode, count ) == true )
		{

/*
			// Test Code
			static char ts[50];
			time_t rawtime;
			rawtime = time(NULL);
			struct tm timeinfo;
			gmtime_s(&timeinfo, &rawtime);
			strftime(ts,50,"%b-%d-%Y %X",&timeinfo);

			char text[80];

			sprintf_s(text,"%s: ***** PMAG New Start (%3.3f)\r\n",ts,pmag );
			func_status(TEXT_STATUS,text);
*/

			return start;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}

	
}
int Cm110s::find_frequency_error_and_start_of_preamble( FComplex *in, float &error, float &pmag, Mode &mode, int &count )
{
	FComplex hi[M1_RX_BLOCK_LENGTH];
	FComplex lo[M1_RX_BLOCK_LENGTH];
	FComplex tp[M1_RX_BLOCK_LENGTH];

	float    mag;
	float    min;
	float    f_offset;
	int      i,start,index;
	float    on_channel_max=0;
	float    lo_channel_max=0;
	float    hi_channel_max=0;
	float    on_channel_min=100000;
	float    lo_channel_min=100000;
	float    hi_channel_min=100000;

	int      on_channel_pos=0;
	int      lo_channel_pos=0;
	int      hi_channel_pos=0;
	int      block_length;


	//BIG BOY
//	if(nr_state == 0)
//	{
		f_offset = (float)((50.0*PI)/(2400.0)); // Charles
//	}
//	else
//	{
//		f_offset = (float)((10.0*PI)/(2400.0));
//	}


	// Take into acount the length of the equaliser 
	index = M1_FF_EQ_LENGTH/2;
	// Input has been decimated by 2
	block_length = M1_RX_BLOCK_LENGTH/2;
	
    /* Do on frequency channel */
        
	for( i=0; i < block_length; i++, index++ )
	{
		mag = correlate_common_preamble( &in[index] );

		m_p_mag[ON_CHANNEL][0][index] = m_p_mag[ON_CHANNEL][1][index];
		m_p_mag[ON_CHANNEL][1][index] = mag;

		// Acumulate the two mags
		mag += m_p_mag[ON_CHANNEL][0][index];

		if( mag > on_channel_max ) 
		{
			on_channel_max    = mag;
			on_channel_pos    = i;
		}
		if( mag < on_channel_min )
		{
			on_channel_min = mag;
		}
	}

	/* Generate a hi channel */
	index = M1_FF_EQ_LENGTH/2;	
	translate_seq_in_freq( in, hi,  f_offset, M1_RX_BLOCK_LENGTH );

	for( i=0; i < block_length; i++, index++ )
	{
		mag = correlate_common_preamble( &hi[index] );

		m_p_mag[HI_CHANNEL][0][index] = m_p_mag[HI_CHANNEL][1][index];
		m_p_mag[HI_CHANNEL][1][index] = mag;

		// Acumulate the two mags
		mag += m_p_mag[HI_CHANNEL][0][index];

		if( mag > hi_channel_max ) 
		{
			hi_channel_max    = mag;
			hi_channel_pos    = i;
		}
		if( mag < hi_channel_min )
		{
			hi_channel_min = mag;
		}
	}

	/* Generate lo channel */
	index = M1_FF_EQ_LENGTH/2;
	translate_seq_in_freq( in, lo, -f_offset, M1_RX_BLOCK_LENGTH );
	
	for( i=0; i < block_length; i++, index++ )
	{
		mag = correlate_common_preamble( &lo[index] );

		m_p_mag[LO_CHANNEL][0][index] = m_p_mag[LO_CHANNEL][1][index];
		m_p_mag[LO_CHANNEL][1][index] = mag;

		// Acumulate the two mags
		mag += m_p_mag[LO_CHANNEL][0][index];

		if( mag > lo_channel_max ) 
		{
			lo_channel_max    = mag;
			lo_channel_pos    = i;
		}
		if( mag < lo_channel_min )
		{
			lo_channel_min = mag;
		}
	}

	if( (on_channel_max > lo_channel_max) && ( on_channel_max > hi_channel_max))
	{
		start   = on_channel_pos+M1_FF_EQ_LENGTH/2;
		error   = calculate_frequency_error( &in[start] );
		pmag    = on_channel_max;
		min     = on_channel_min;
		max_p_channel = ON_CHANNEL;
	}
	else
	{
		if(( hi_channel_max > lo_channel_max ) && ( hi_channel_max > on_channel_max))
		{
			start   = hi_channel_pos+M1_FF_EQ_LENGTH/2;
			error   = calculate_frequency_error( &hi[start] )+f_offset;
			pmag    = hi_channel_max;
			min     = hi_channel_min;
			max_p_channel = HI_CHANNEL;
		}
		else
		{
			start   = lo_channel_pos+M1_FF_EQ_LENGTH/2;
			error   = calculate_frequency_error( &lo[start] )-f_offset;
			pmag    = lo_channel_max;
			min     = lo_channel_min;
			max_p_channel = LO_CHANNEL;
		}		
	}
	



	// ADDED TEST
	if( tx_state == TX_IDLE_STATE )
	{
		float d_error;
		static float  delta_error;
		delta_error            = error;
		d_error        = (float)(-delta_error*2400.0/PI);
		func_status( FREQ_ERROR_STATUS, &d_error );
		lastderror = d_error;
	}


	// During preamble hunt
	//Update SNR for display in db
	float snr;
	snr = (float) 20.0*(log10(pmag*2));
	//snr = (float) 10.0*(log10(pmag));
	//snr = (float) 10.0*(log10(pmag/2));
	snr = ((snr+lasthuntsnr)/2.5f)-1.5f; //2
	if (isnan(snr) || isinf(snr))
	{
		func_status(SNR_STATUS, &lasthuntsnr);
	}
	else
	{
		func_status(SNR_STATUS, &snr);//HUNT
	}
	lasthuntsnr = snr;


	//if( pmag > 10.0*min ) < Charles
	// Test in SDT v1.0.1 BUILD 1.0.1.0
	//if( pmag > 5.0*min ) < was using this in v1.01 b1.0.1.1. and v1.01 b1.0.1.2. but lots of falsing
	//if( pmag > 8.0*min ) < used this until PMAG Squelch introduced in v1.01 b1.0.1.4 TB#6.6


	// Front End Gate when SQL LO or HI

	// Preamble Magnitude
	if( pmag <= 8.0*min )	
	{
		return -1;
	}

/*
	if(pmag_multiplier >= 9.0)
	{
		// Preamble Magnitude
		if( pmag <= pmag_multiplier*min )	
		{
			return -1;
		}
	}
*/



	//if( pmag > pmag_sql_factor*min )

	pmag_multiplier = pmag_sql_factor;

	//if( pmag > pmag_multiplier*min )
	
	//pmag_multiplier = pmag_multiplier*0.125;
	// SQL OFF = 1.0
	// SQL LOW = 1.125
	// SQL HI  = 1.25



	if(pmag_multiplier == 8.0)
	{
		pmag_multiplier = 0.50; // SQL OFF
	}

	if(pmag_multiplier == 9.0)
	{
		pmag_multiplier = 2.0;	//SQL LO 2db more signal required
	}

	if(pmag_multiplier == 10.0)
	{
		pmag_multiplier = 3.0;	//SQL HI 5db more signal required	
	}
	// Future to do of SQL 1..10 selections as a 1:1 for pmag_multiplier
	// where SQL OFF will result in pmag_multiplier of 0.50 ?



	if( pmag > pmag_multiplier )
	{

		/*		
		char datext[80];

		sprintf_s(datext," ***** pmag (%4.3f)\r\n", pmag );
		func_status(TEXT_STATUS,datext);
		*/

		/*
		sprintf_s(datext," ***** pmag_multiplier SQL HI/LO (%4.3f)\r\n", pmag_multiplier*min );
		func_status(TEXT_STATUS,datext);
		*/

		
		/*
		sprintf_s(datext," ***** pmag_multiplier (%4.3f)\r\n", pmag_multiplier );
		func_status(TEXT_STATUS,datext);
		*/	

		// During preamble decoding
		//Update SNR for display in db
		float snr;
		//snr = (float) 20.0*(log10(pmag));
		snr = (float) 10.0*(log10(pmag/2));
		//snr = (snr+lastpreamblesnr)/2;
		if (isnan(snr) || isinf(snr))
		{
			func_status(SNR_STATUS, &lastsnr);
		}
		else
		{
			func_status(SNR_STATUS, &snr);
		}
		if (isnan(snr) || isinf(snr))
		{
			func_status(SNR_STATUS, &lastpreamblesnr);
		}
		else
		{
			func_status(PREAM_SNR_STATUS, &snr); // PREAMBLE
		}
		preamsnr = snr;
		lastpreamblesnr = snr;

		/* Correct the information bearing section */
		translate_seq_in_freq( &in[start+(M1_P_COMMON_LENGTH*2)], tp, error, (M1_P_MODE_LENGTH+M1_P_COUNT_LENGTH)*2 );

		// ADDED TEST
		if( tx_state == TX_IDLE_STATE )
		{		
			float d_error;
			static float  delta_error;
			delta_error            = error;
			d_error        = (float)(-delta_error*2400.0/PI);
			func_status( FREQ_ERROR_STATUS, &d_error );
			lastderror = d_error;

		}

		/* Find mode parameters */
		mode   = find_mode_value( tp );

		/* Find count parameters */
		count  = find_count_value( &tp[M1_P_MODE_LENGTH*2] );		


		// FAST EXIT ON FAIL
		if( verify_mode( mode, count ) == false )
		{
			return -1;
		}


		if(nr_state == 0)
		{

			// ORIGINAL CODE
			if( verify_mode( mode, count ) == true )
			{

				float d_error;
				static float  delta_error;
				delta_error    = error;
				d_error        = (float)(-delta_error*2400.0/PI);

				// Exit on Frequency Error too far out
				if(d_error>= 81.1) return -1;
				if(d_error<= -81.1) return -1;

				return start;
			}
			else
			{
				return -1;
			}

	}
	else
	{

		/*

		// CODE TESTING ROUTINES
		if( verify_mode( mode, count ) == true )
		{

			float d_error;
			static float  delta_error;
			delta_error    = error;
			d_error        = (float)(-delta_error*2400.0/PI);

			static char ts[50];
			time_t rawtime;
			rawtime = time(NULL);
			struct tm timeinfo;
			gmtime_s(&timeinfo, &rawtime);
			strftime(ts,50,"%b-%d-%Y %X",&timeinfo);

			char text[80];

			// Less than 2.0 is false signal
			//if(pmag<= 2.0) return -1;
		
			// Frequency Error too far out
			if(d_error>= 80.1) return -1;
			if(d_error<= -80.1) return -1;

			// Test code to Stop SHORT and 4800bps FALSE detects
			switch( mode )
			{
                                   
				case M75NS:	
					if(preamsnr <=-15)
					{
						sprintf_s(text,"%s: ***** PMAG (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 75Sbps Verify Freq. Error: (%3.1f hz.)\r\n",ts,d_error );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 75Sbps Verify SNR: (%3.0f db)\r\n",ts,preamsnr );
						func_status(TEXT_STATUS,text);

						return -1;
					}
					else
					{
						sprintf_s(text,"%s: ***** 75S PMAG START (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						return start;
					}
					break;
				case M75NL:
					if(preamsnr <=-15)
					{

						sprintf_s(text,"%s: ***** PMAG (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 75Lbps Verify Freq. Error: (%3.1f hz.)\r\n",ts,d_error );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 75Lbps Verify SNR: (%3.0f db)\r\n",ts,preamsnr );
						func_status(TEXT_STATUS,text);

						return -1;
					}
					else
					{
						sprintf_s(text,"%s: ***** 75L PMAG START (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						return start;
					}
					break;
				case M150S:
					//if((preamsnr <=-2 && d_error>=10)||(preamsnr <=-2 && d_error<= -10)) return -1;
                    //if(preamsnr <=-2) return -1;
					if(preamsnr <=-2) 
					{

						sprintf_s(text,"%s: ***** PMAG (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 150Sbps Verify Freq. Error: (%3.1f hz.)\r\n",ts,d_error );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 150Sbps Verify SNR: (%3.0f db)\r\n",ts,preamsnr );
						func_status(TEXT_STATUS,text);
					
						return -1;
					}
					else
					{
						sprintf_s(text,"%s: ***** 150S PMAG START (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						return start;
					}
					break;                                   
				case M150L:
					//if((preamsnr <=-2 && d_error>=10)||(preamsnr <=-2 && d_error<= -10)) return -1;
                    //if(preamsnr <=-2) return -1;
					if(preamsnr <=-2) 
					{

						sprintf_s(text,"%s: ***** PMAG (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 150Lbps Verify Freq. Error: (%3.1f hz.)\r\n",ts,d_error );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 150Lbps Verify SNR: (%3.0f db)\r\n",ts,preamsnr );
						func_status(TEXT_STATUS,text);
	
						return -1;
					}
					else
					{
						sprintf_s(text,"%s: ***** 150L PMAG START (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						return start;
					}
					break;
                case M300S:
					//if((preamsnr <=0 && d_error>=10)||(preamsnr <=0 && d_error<= -10)) return -1;
					//if(preamsnr <=0) return -1;
					if(preamsnr <=0)
					{

						sprintf_s(text,"%s: ***** PMAG (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 300Sbps Verify Freq. Error: (%3.1f hz.)\r\n",ts,d_error );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 300Sbps Verify SNR: (%3.0f db)\r\n",ts,preamsnr );
						func_status(TEXT_STATUS,text);											

						return -1;

					}
					else
					{
						sprintf_s(text,"%s: ***** 300S PMAG START (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						return start;
					}											
					break;                                   
				case M300L:
					//if((preamsnr <=0 && d_error>=10)||(preamsnr <=0 && d_error<= -10)) return -1;
					//if(preamsnr <=0) return -1;
                    if(preamsnr <=0)
					{

						sprintf_s(text,"%s: ***** PMAG (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 300Lbps Verify Freq. Error: (%3.1f hz.)\r\n",ts,d_error );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 300Lbps Verify SNR: (%3.0f db)\r\n",ts,preamsnr );
						func_status(TEXT_STATUS,text);											

						return -1;

					}
					else
					{
					
						sprintf_s(text,"%s: ***** 300L PMAG START (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						return start;
					}											
					break;                                  
				case M600S:
					//if((preamsnr <=5 && d_error>=10)||(preamsnr <=5 && d_error<= -10)) return -1;
					//if(preamsnr <=5) return -1;
					if(preamsnr <=5)
					{
						sprintf_s(text,"%s: ***** PMAG (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 600Sbps Verify Freq. Error: (%3.1f hz.)\r\n",ts,d_error );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 600Sbps Verify SNR: (%3.0f db)\r\n",ts,preamsnr );
						func_status(TEXT_STATUS,text);

						return -1;
					}
					else
					{
						sprintf_s(text,"%s: ***** 600S PMAG START (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						return start;
					}
					break;
				case M600L:
					//if((preamsnr <=5 && d_error>=10)||(preamsnr <=5 && d_error<= -10)) return -1;
					//if(preamsnr <=5) return -1;
					if(preamsnr <=5)
					{
						sprintf_s(text,"%s: ***** PMAG (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 600Lbps Verify Freq. Error: (%3.1f hz.)\r\n",ts,d_error );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 600Lbps Verify SNR: (%3.0f db)\r\n",ts,preamsnr );
						func_status(TEXT_STATUS,text);

						return -1;
					}
					else
					{
						sprintf_s(text,"%s: ***** 600L PMAG START (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						return start;
					}
                                       
					break;                                   
				case M1200S:
					//if((preamsnr <=9 && d_error>=10)||(preamsnr <=9 && d_error<= -10)) return -1;
					//if(preamsnr <=9) return -1;
					if(preamsnr <=9)
					{
						sprintf_s(text,"%s: ***** PMAG (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 1200Sbps Verify Freq. Error: (%3.1f hz.)\r\n",ts,d_error );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 1200Sbps Verify SNR: (%3.0f db)\r\n",ts,preamsnr );
						func_status(TEXT_STATUS,text);

						return -1;
					}
					else
					{
						sprintf_s(text,"%s: ***** 1200S PMAG START (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						return start;
					}
					break;
				case M1200L:
					//if((preamsnr <=9 && d_error>=10)||(preamsnr <=9 && d_error<= -10)) return -1;
					//if(preamsnr <=9) return -1;
					if(preamsnr <=9)
					{
						sprintf_s(text,"%s: ***** PMAG (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 1200Lbps Verify Freq. Error: (%3.1f hz.)\r\n",ts,d_error );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 1200Lbps Verify SNR: (%3.0f db)\r\n",ts,preamsnr );
						func_status(TEXT_STATUS,text);

						return -1;
					}
					else
					{
						sprintf_s(text,"%s: ***** 1200L PMAG START (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						return start;
					}
					break;
				case M2400S:
					//if((preamsnr <=8 && d_error>=10)||(preamsnr <=8 && d_error<= -10)) return -1;
					//if(preamsnr <=8) return -1;
					if(preamsnr <=8)
					{
						sprintf_s(text,"%s: ***** PMAG (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 2400Sbps Verify Freq. Error: (%3.1f hz.)\r\n",ts,d_error );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 2400Sbps Verify SNR: (%3.0f db)\r\n",ts,preamsnr );
						func_status(TEXT_STATUS,text);

						return -1;
					}
					else
					{
						sprintf_s(text,"%s: ***** 2400S PMAG START (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						return start;
					}                                            
					break;
				case M2400L:
					//if((preamsnr <=8 && d_error>=10)||(preamsnr <=8 && d_error<= -10)) return -1;
					//if(preamsnr <=8) return -1;
					if(preamsnr <=8)
					{
						sprintf_s(text,"%s: ***** PMAG (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 2400Lbps Verify Freq. Error: (%3.1f hz.)\r\n",ts,d_error );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 2400Lbps Verify SNR: (%3.0f db)\r\n",ts,preamsnr );
						func_status(TEXT_STATUS,text);

						return -1;
					}
					else
					{
						sprintf_s(text,"%s: ***** 2400L PMAG START (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						return start;
					}                                            
					break;                                 
				case M2400V:
					//if((preamsnr <=8 && d_error>=10)||(preamsnr <=8 && d_error<= -10)) return -1;                                        
					//if(preamsnr <=8) return -1;
					if(preamsnr <=8)
					{

						sprintf_s(text,"%s: ***** PMAG (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 2400Vbps Verify Freq. Error: (%3.1f hz.)\r\n",ts,d_error );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 2400Vbps Verify SNR: (%3.0f db)\r\n",ts,preamsnr );
						func_status(TEXT_STATUS,text);

						return -1;
					}
					else
					{
						sprintf_s(text,"%s: ***** 2400V PMAG START (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						return start;
					}
					break;                                   
				case M4800S:
					//if((preamsnr <=10 && d_error>=10)||(preamsnr <=10 && d_error<= -10)) return -1;                                           
					//if(preamsnr <=10) return -1;
					if(preamsnr <=10 )
					{
						sprintf_s(text,"%s: ***** PMAG (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 4800bps Verify Freq. Error: (%3.1f hz.)\r\n",ts,d_error );
						func_status(TEXT_STATUS,text);

						sprintf_s(text,"%s: ***** 4800bps Verify SNR: (%3.0f db)\r\n",ts,preamsnr );
						func_status(TEXT_STATUS,text);

						return -1;

					}
					else
					{
						sprintf_s(text,"%s: ***** 4800S PMAG START (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);

						return start;
					}                                           
					break;                                   
				default:
						sprintf_s(text,"%s: ***** default: PMAG START (%6.1f)\r\n",ts,pmag );
						func_status(TEXT_STATUS,text);
                                            
						return -1;                                            
					break;

			}// end SWITCH
                          
					//return start;
                 
		}
        else              
		{                  
			return -1;           
		}

		// End Code Test Routines
		
		*/


		// Fielded Approach
		if( verify_mode( mode, count ) == true )
		{

			float d_error;
			static float  delta_error;
			delta_error    = error;
			d_error        = (float)(-delta_error*2400.0/PI);

        //	static char ts[50];
        //	time_t rawtime;
        //	rawtime = time(NULL);
        //	struct tm timeinfo;
        //	gmtime_s(&timeinfo, &rawtime);
        //	strftime(ts,50,"%b-%d-%Y %X",&timeinfo);

			//char text[80];

			// Exit on Frequency Error too far out
			if(d_error>= 81.1) return -1;
			if(d_error<= -81.1) return -1;

			// Test code to Stop SHORT and 4800bps FALSE detects
			switch( mode )
			{                                   
				case M75NS:	
					if((preamsnr <=-15)||(pmag<= 0.99))
					{
						return -1;
					}
					else
					{

						//sprintf_s(text,"%s: ***** 75S PMAG START (%3.3f)\r\n",ts,pmag );
						//func_status(TEXT_STATUS,text);

						return start;
					}
					break;
				case M75NL:
					if((preamsnr <=-15)||(pmag<= 0.99))
					{
						return -1;
					}
					else
					{
						//sprintf_s(text,"%s: ***** 75L PMAG START (%3.3f)\r\n",ts,pmag );
						//func_status(TEXT_STATUS,text);

						return start;
					}
					break;
				case M150S:
					if((preamsnr <=-2)||(pmag<= 1.50))
					{
						return -1;
					}
					else
					{
						//sprintf_s(text,"%s: ***** 150S PMAG START (%3.3f)\r\n",ts,pmag );
						//func_status(TEXT_STATUS,text);

						return start;
					}
					break;                                   
				case M150L:
					if((preamsnr <=-2)||(pmag<= 1.50))
					{
						return -1;
					}
					else
					{

						//sprintf_s(text,"%s: ***** 150L PMAG START (%3.3f)\r\n",ts,pmag );
						//func_status(TEXT_STATUS,text);

						return start;
					}
					break;
                case M300S:
					if((preamsnr <=0)||(pmag<= 2.0))
					{
						return -1;
					}
					else
					{
						//sprintf_s(text,"%s: ***** 300S PMAG START (%3.3f)\r\n",ts,pmag );
						//func_status(TEXT_STATUS,text);

						return start;
					}											
					break;                                   
				case M300L:
					if((preamsnr <=0)||(pmag<= 2.0))
					{
						return -1;
					}
					else
					{

						//sprintf_s(text,"%s: ***** 300L PMAG START (%3.3f)\r\n",ts,pmag );
						//func_status(TEXT_STATUS,text);

						return start;
					}											
					break;                                  
				case M600S:
					//if((preamsnr <=5)||(pmag<= 2.0))
					if(pmag<= 2.0)
					{
						return -1;
					}
					else
					{

						//sprintf_s(text,"%s: ***** 600S PMAG START (%3.3f)\r\n",ts,pmag );
						//func_status(TEXT_STATUS,text);

						return start;
					}
					break;
				case M600L:
					//if((preamsnr <=0)||(pmag<= 6.0))
					if(pmag<= 2.0)
					{
						return -1;
					}
					else
					{

						//sprintf_s(text,"%s: ***** 600L PMAG START (%3.3f)\r\n",ts,pmag );
						//func_status(TEXT_STATUS,text);

						return start;
					}                                       
					break;  
				case M600V:
					//if((preamsnr <=5)||(pmag<= 2.0))
					if (pmag <= 2.0)
					{
						return -1;
					}
					else
					{

						//sprintf_s(text,"%s: ***** 600V PMAG START (%3.3f)\r\n",ts,pmag );
						//func_status(TEXT_STATUS,text);

						return start;
					}
					break;
				case M1200S:
					//if((preamsnr <=9)||(pmag<= 2.0))
					if(pmag<= 2.0)
					{
						return -1;
					}
					else
					{

						//sprintf_s(text,"%s: ***** 1200S PMAG START (%3.3f)\r\n",ts,pmag );
						//func_status(TEXT_STATUS,text);

						return start;
					}
					break;
				case M1200L:
					//if((preamsnr <=9)||(pmag<= 8.0))
					if(pmag<= 2.0)
					{
						return -1;
					}
					else
					{

						//sprintf_s(text,"%s: ***** 1200L PMAG START (%3.3f)\r\n",ts,pmag );
						//func_status(TEXT_STATUS,text);
						
						return start;
					}
					break;
				case M1200V:
					//if((preamsnr <=9)||(pmag<= 2.0))
					if (pmag <= 2.0)
					{
						return -1;
					}
					else
					{

						//sprintf_s(text,"%s: ***** 1200V PMAG START (%3.3f)\r\n",ts,pmag );
						//func_status(TEXT_STATUS,text);

						return start;
					}
					break;
				case M2400S:
					//if((preamsnr <=8)||(pmag<= 8.0))
					//if(preamsnr <=8)
					if(pmag<= 2.0)
					{
						return -1;
					}
					else
					{
						//sprintf_s(text,"%s: ***** 2400S PMAG START (%3.3f)\r\n",ts,pmag );
						//func_status(TEXT_STATUS,text);

						return start;
					}                                            
					break;
				case M2400L:
					//if((preamsnr <=8)||(pmag<= 8.0))
					//if(preamsnr <=8)
					if(pmag<= 2.0)
					{
						return -1;
					}
					else
					{
						//sprintf_s(text,"%s: ***** 2400L PMAG START (%3.3f)\r\n",ts,pmag );
						//func_status(TEXT_STATUS,text);

						return start;
					}                                            
					break;                                 
				case M2400V:
					//if((preamsnr <=8)||(pmag<= 8.0))
					//if(preamsnr <=8)
					if(pmag<= 2.0)
					{
						return -1;
					}
					else
					{
						//sprintf_s(text,"%s: ***** 2400V PMAG START (%3.3f)\r\n",ts,pmag );
						//func_status(TEXT_STATUS,text);

						return start;
					}
					break;                                   
				case M4800S:
					//if(preamsnr <=6)
					if(pmag<= 10.0)
					{
						return -1;
					}
					else
					{

						//sprintf_s(text,"%s: ***** 4800S PMAG START (%3.3f)\r\n",ts,pmag );
						//func_status(TEXT_STATUS,text);
						
						return start;
					}                                           
					break;                                   
				default:                                         
						return -1;                                            
					break;

			}// end SWITCH
                          
					//return start;
                 
		}
        else              
		{                  
			return -1;           
		}

		// End Fielded Approach



	} // end nr_state

	}
	else
	{
		// We fall through here Hunting PREAMBLE when there is no MS110A signal
		
		// PMAG is less than 1.0 for a FAIL
		/*
			static char ts[50];
			time_t rawtime;
			rawtime = time(NULL);
			struct tm timeinfo;
			gmtime_s(&timeinfo, &rawtime);
			strftime(ts,50,"%b-%d-%Y %X",&timeinfo);
			char text[80];


			sprintf_s(text,"%s: ***** PMAG FAIL (%3.3f)\r\n",ts,pmag );
			func_status(TEXT_STATUS,text);
				
		*/

		return -1;
	}




}
void Cm110s::process_receive_block( float *in )
{
    //return;
    //qDebug()<<Q_FUNC_INFO<<"Entered";
    int i;
	float         sample,error,d_error,cmag;
	static float  acc;
	static float  delta_error;
	static float  cmag_max;
	static float  cmag_multiplier;
	static int    last_start;
	static Mode   last_mode;
	static int    last_count;
	static int    rx_preamble_down_count;
	static int    rx_preamble_state_count;
	static int    rx_preamble_state_count_test;
	static int    best_start;
	static int    preamble_errors;
	int           new_start;
	Mode          new_mode;
	int           new_count;
		
	if( m_rx_enable == false ) return;

	/* Down convert */
	for( i = 0; i < M1_RX_BLOCK_LENGTH; i++ )
	{
		b1[i] =  b1[i+M1_RX_BLOCK_LENGTH];
		/* Update with new samples */
		sample = in[i];
		b1[i+M1_RX_BLOCK_LENGTH].re =    (float)cos(acc)*sample;
		b1[i+M1_RX_BLOCK_LENGTH].im =   -(float)sin(acc)*sample;

		//acc         +=  (float)((2*PI*M1_CENTER_FREQUENCY)/M1_SAMPLE_RATE);

		if(psk_carrier_select == 1800)
		{
			acc         +=  (float)((2*PI*M1_CENTER_FREQUENCY)/M1_SAMPLE_RATE);
		}
		else if(psk_carrier_select == 1650)
		{
			acc         +=  (float)((2*PI*M2_CENTER_FREQUENCY)/M1_SAMPLE_RATE);
		}
		else if(psk_carrier_select == 1500)
		{
			acc         +=  (float)((2*PI*M3_CENTER_FREQUENCY)/M1_SAMPLE_RATE);
		}
		else
		{
			acc         +=  (float)((2*PI*M1_CENTER_FREQUENCY)/M1_SAMPLE_RATE);
		}


		if( acc >= (float)(2*PI) ) acc -= (float)(2*PI);

		//Too much gain when enabled all the time as originally coded
		//about 30db!
		//b1[i+M1_RX_BLOCK_LENGTH] = agc(b1[i+M1_RX_BLOCK_LENGTH]);	

		//BIG BOY
		
		if(agc_state == 1) // 0 = AGC OFF
		{
			// HIGH AGC
			b1[i+M1_RX_BLOCK_LENGTH] = agc_2(b1[i+M1_RX_BLOCK_LENGTH]);
		}

		if(agc_state == 2)
		{
			// LOW AGC
			b1[i+M1_RX_BLOCK_LENGTH] = agc(b1[i+M1_RX_BLOCK_LENGTH]);		
		}



		/*
		// ADDED Test
		//switch( tx_mode ) // ERROR should be RX
		switch( rx_mode )
		{
			case M75NS:
			case M75NL:
			case M150S:
			case M150L:
			case M300S:
			case M300L:
			case M600S:
			case M600L:
				break;
			case M1200S:
			case M1200L:
				//b1[i+M1_RX_BLOCK_LENGTH] = agc(b1[i+M1_RX_BLOCK_LENGTH]);
				break;
			case M2400S:
			case M2400L:
			case M2400V:
				break;
			case M4800S:
				//b1[i+M1_RX_BLOCK_LENGTH] = agc(b1[i+M1_RX_BLOCK_LENGTH]);
				break;
			default:
				break;
		}
		*/

	}
	/* Filter and decimate by two */
	for( i = 0; i < M1_RX_BLOCK_LENGTH; i++ )
	{
		b2[i] =  b2[i+M1_RX_BLOCK_LENGTH/2];
	}

	for( i = 0; i < M1_RX_BLOCK_LENGTH/2; i++ )
	{
		/* Update with new samples */
		b2[i+M1_RX_BLOCK_LENGTH] =  filter( &b1[i*2], rx_coffs, M1_RX_FILTER_LENGTH );
	}
    //qDebug()<<Q_FUNC_INFO<<"Before switch on state";
    //return;
    /* Now do post processing */
	switch( rx_state )
	{
		case RX_HUNTING_PREAMBLE_STATE:
		    cmag_max    = 0;
			delta_error = 0;
		    rx_preamble_down_count = - 1; /* Cannot be valid at this point */
			rx_preamble_state_count = 0;

		    /* Find the start of the preamble */
			new_start = find_frequency_error_and_start_of_preamble( b2, error, cmag, new_mode, new_count );
			if( new_start != -1 )
			{
				printf("[RX] Preamble candidate: start=%d mode=%d count=%d cmag=%.2f\n", new_start, new_mode, new_count, cmag);
				printf("[RX] Checking: new_mode=%d last_mode=%d new_count=%d last_count=%d\n", new_mode, last_mode, new_count, last_count);
				if(( new_mode == last_mode ) && ( new_count == last_count-1 ) )
				{
					printf("[RX] >>> PREAMBLE LOCKED! DCD TRUE <<<\n"); 
					/* At the start of the frame reset everything */
					delta_error            = error;
					rx_preamble_down_count = new_count;
					cmag_max               = cmag;
					best_start             = new_start;
					d_error                = (float)(-delta_error*2400.0/PI);
					set_rx_mode( new_mode );
					func_status( FREQ_ERROR_STATUS, &d_error );
					lastderror = d_error;

					func_status( DCD_TRUE_STATUS, NULL );
					preamble_errors        = 0;
			 		rx_state               = RX_PREAMBLE_FOUND_STATE;

                //	static char ts[50];
                //	time_t rawtime;
                //	rawtime = time(NULL);
                //	struct tm timeinfo;
                //	gmtime_s(&timeinfo, &rawtime);
                //	strftime(ts,50,"%b-%d-%Y %X",&timeinfo);

                //	char text[80];

                //	sprintf_s(text,"%s: Frame Start Position = %i\r\n",ts,frame_start_position );
                //	func_status(TEXT_STATUS,text);

                //	sprintf_s(text,"%s: Preamble Found\r\n",ts);
                //	func_status(TEXT_STATUS,text);

                //	sprintf_s(text,"%s: Preamble Autobaud: %s\r\n",ts,display_mode_string( rx_mode ));
                //	func_status(TEXT_STATUS,text);

					/*
					switch (rx_mode)
					{

						case M600V:
						case M1200V:
						case M2400V:
							//Hardware Modem ASYNC Decoder Force Reset
							m_eomreset = 0; // new eom reset
							float async_decoder_reset;
							async_decoder_reset = 5555.0;
							func_status(TEXT_STATUS, &async_decoder_reset);
							break;
						default:
							break;
					}
					*/

					if(pmag_sql_factor>=9)
					{
						if(pmag_sql_factor==10)
						{
                    //		sprintf_s(text,"%s: SQUELCH HIGH\r\n",ts);
                    //		func_status(TEXT_STATUS,text);
						}

						if(pmag_sql_factor==9)
						{
                        //	sprintf_s(text,"%s: SQUELCH LOW\r\n",ts);
                        //	func_status(TEXT_STATUS,text);
						}

					}
					else
					{
                    //	sprintf_s(text,"%s: SQUELCH OFF\r\n",ts);
                    //	func_status(TEXT_STATUS,text);
					}



                //	sprintf_s(text,"%s: Preamble Freq. Error: (%3.1f hz.)\r\n",ts,d_error );
                //	func_status(TEXT_STATUS,text);

					//sprintf_s(text,"%s: Preamble SNR: (%3.0f db)\r\n",ts,preamsnr );
					//func_status(TEXT_STATUS,text);

					if (isnan(preamsnr) || isinf(preamsnr))
					{
						float dummysnr = 0.0;
                    //	sprintf_s(text, "%s: Preamble SNR: (%3.0f db)\r\n", ts, dummysnr);
                    //	func_status(TEXT_STATUS, text);
					}
					else
					{
                    //	sprintf_s(text, "%s: Preamble SNR: (%3.0f db)\r\n", ts, preamsnr);
                    //	func_status(TEXT_STATUS, text);
					}



					state_text();
				}			
				last_start = new_start;
				last_mode  = new_mode;
				last_count = new_count;
			}			
			break;
		case RX_PREAMBLE_FOUND_STATE:

        //	static char ts[50];
        //	time_t rawtime;
        //	rawtime = time(NULL);
        //	struct tm timeinfo;
        //	gmtime_s(&timeinfo, &rawtime);
        //	strftime(ts,50,"%b-%d-%Y %X",&timeinfo);

			if( rx_preamble_down_count == 22 )
			{
        //		char text[80];
        //		sprintf_s(text,"%s: PREAM%i\r\n",ts,rx_preamble_down_count+1);
				// Send to MS-DMT Scrolling Status Window 
        //		func_status(TEXT_STATUS,text);
			}


			if( rx_mode == M4800S || rx_mode == M75NS || rx_mode == M150S || rx_mode == M300S || rx_mode == M600S || rx_mode == M600V || rx_mode == M1200S || rx_mode == M1200V || rx_mode == M2400S || rx_mode == M2400V )
			{
				if( rx_preamble_down_count == 1 )
				{
                //	char text[80];
                //	sprintf_s(text,"%s: PREAM%i\r\n",ts,rx_preamble_down_count+1);
					// Send to MS-DMT Scrolling Status Window 
                //	func_status(TEXT_STATUS,text);
				}
			
			}

            char text[80];
        //	sprintf_s(text,"%s: PREAM%i\r\n",ts,rx_preamble_down_count);
			// Send to MS-DMT Scrolling Status Window 
        //	func_status(TEXT_STATUS,text);

			// Count the PREAM states where there are 22 for all but
			// 4800bps for which there is just 1.
			if( rx_preamble_down_count == 22 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 21 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 20 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 19 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 18 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 17 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 16 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 15 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 14 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 13 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 12 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 11 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 10 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 9 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 8 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 7 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 6 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 5 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 4 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 3 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 2 ) rx_preamble_state_count++;
			if( rx_preamble_down_count == 1 ) rx_preamble_state_count++;

			if( rx_mode == M4800S || rx_mode == M75NS || rx_mode == M150S || rx_mode == M300S || rx_mode == M600S || rx_mode == M600V || rx_mode == M1200S || rx_mode == M1200V || rx_mode == M2400S || rx_mode == M2400V  )
			{
				if( rx_preamble_down_count == 1 )
				{
                    sprintf(text,"PREAM%i\r\n",rx_preamble_down_count+1);
					// Send to MS-DMT State Window
                    sprintf( m_state_text,text);
				}			
			}


			if( rx_preamble_down_count == 22 )
			{
                sprintf(text,"PREAM%i\r\n",rx_preamble_down_count+1);
				// Send to MS-DMT State Window
                sprintf( m_state_text,text);
			}


            sprintf(text,"PREAM%i\r\n",rx_preamble_down_count);
			// Send to MS-DMT State Window
            sprintf( m_state_text,text);

			if( rx_preamble_down_count == 1 )
			{
                sprintf( m_state_text,"DATA");
			}

			/* Decrement down counter */
			rx_preamble_down_count--;

			/* Analyse the preamble block */
			new_start = new_start_find_frequency_error_and_start_of_preamble( b2, error, cmag, new_mode, new_count );			

			/* Try to reduce false syncs */
			//if( new_mode != last_mode )               preamble_errors++;
			//if( new_count != rx_preamble_down_count ) preamble_errors++;

			if( new_mode != last_mode )
			{
        //		strftime(ts,50,"%b-%d-%Y %X",&timeinfo);
				preamble_errors++;
        //		sprintf_s(text,"%s: Preamble errors: %i\r\n",ts,preamble_errors);
        //		func_status(TEXT_STATUS,text);
            }

			if( new_count != rx_preamble_down_count ) 
			{
        //		strftime(ts,50,"%b-%d-%Y %X",&timeinfo);
				preamble_errors++;
        //		sprintf_s(text,"%s: Preamble errors: %i\r\n",ts,preamble_errors);
        //		func_status(TEXT_STATUS,text);
			}			

			if( preamble_errors > 6 )
			{
        //		strftime(ts,50,"%b-%d-%Y %X",&timeinfo);
				//char text[80];
        //		sprintf_s(text,"%s: Preamble errors: %i\r\n",ts,preamble_errors);
        //		func_status(TEXT_STATUS,text);
			}

			if((new_mode == last_mode ) && ( new_count == rx_preamble_down_count ))
			{
				preamble_errors = 0;

				/*
				strftime(ts,50,"%b-%d-%Y %X",&timeinfo);

				char text[80];
				sprintf_s(text,"%s: Preamble Error Reset \r\n",ts);
				func_status(TEXT_STATUS,text);
				*/
			}

			//if( preamble_errors > 4 ) // Charles
			//if( preamble_errors > 6 ) // change to 6 in v1.01 b1.0.1.3.
			// false_detect_factor follows SQL if OFF = 6, LO = 5 and HI = 4
			if( preamble_errors > false_detect_factor )
			{ 
				func_status( DCD_FALSE_STATUS, NULL );
				rx_state = RX_HUNTING_PREAMBLE_STATE;
                sprintf( m_state_text,"NO DCD");
				state_text();

            //	strftime(ts,50,"%b-%d-%Y %X",&timeinfo);
            //	char text[80];
            //	sprintf_s(text,"%s: Preamble errors: NO DCD \r\n",ts);
            //	func_status(TEXT_STATUS,text);

				rx_reset();

			}
			else
			{


				/* See if we have a better start position for the preamble */
				//if(( cmag > cmag_max*1.5) && (new_start != -1 ) )

/*
				//if(pmag_multiplier >= 10.0) //SQL HI
				if((pmag_multiplier >= 10.0)||(agc_state == 1)) //SQL HI
				{
					cmag_multiplier = 0.50;
				}
				else
				{
					cmag_multiplier = 1.5;
				}
*/
					cmag_multiplier = 1.5;

				if(( cmag > cmag_max*cmag_multiplier) && (new_start != -1 ) )
				{
					cmag_max   = cmag;
					best_start = new_start;
					last_start = new_start;
					delta_error = (delta_error+error)/2;

					/*
					//BIG BOY
					strftime(ts,50,"%b-%d-%Y %X",&timeinfo);
					char text[80];
					sprintf_s(text,"%s: Preamble Delta Error = %3.1fhz\r\n",ts,delta_error);
					func_status(TEXT_STATUS,text);
					*/

                //	strftime(ts,50,"%b-%d-%Y %X",&timeinfo);
					//char text[80];
                //	sprintf_s(text,"%s: Preamble Restart \r\n",ts);
                //	func_status(TEXT_STATUS,text);
	
				}

				/* See if we have received the last preamble before the start of the data */

				if(( new_count == 0 ) || ( rx_preamble_down_count == 0 ))
				{
					reset_frame_start( best_start );
					kalman_init();
					remove_frequency_error( b2, delta_error, M1_RX_BLOCK_LENGTH+(M1_RX_BLOCK_LENGTH/2));
					//if( verify_preamble( &b2[get_frame_start()] ) == false )
					if( verify_preamble( &b2[best_start-M1_FF_EQ_LENGTH/2] ) == false )				
					{
						func_status( DCD_FALSE_STATUS, NULL );
						rx_state = RX_HUNTING_PREAMBLE_STATE;
                        sprintf( m_state_text,"NO DCD");
						state_text();

                    //	strftime(ts,50,"%b-%d-%Y %X",&timeinfo);
                    //	char text[80];

						//sprintf_s(text,"%s: Preamble Fail SNR: (%3.0f db)\r\n",ts,preamsnr );
						//func_status(TEXT_STATUS,text);

						if (isnan(preamsnr) || isinf(preamsnr))
						{
							float dummysnr = 0.0;
                        //	sprintf_s(text, "%s: Preamble Fail SNR: (%3.0f db)\r\n", ts, dummysnr);
                        //	func_status(TEXT_STATUS, text);
						}
						else
						{
                        //	sprintf_s(text, "%s: Preamble Fail SNR: (%3.0f db)\r\n", ts, preamsnr);
                        //	func_status(TEXT_STATUS, text);
						}


                    //	sprintf_s(text,"%s: Preamble fail: NO DCD \r\n",ts);
                    //	func_status(TEXT_STATUS,text);

						rx_preamble_state_count = 0;

					}
					else
					{
						if ((last_mode == M75NS) || (last_mode == M75NL))
						{
							// 75 BPS
							reset_sync_75_mask();
						}
						else
						{
							train_on_preamble(&b2[get_frame_start()], last_mode, 0);
						}
						rx_block_count = 0;
						
						d_error = (float)(-delta_error * 2400.0 / PI);
						func_status(FREQ_ERROR_STATUS, &d_error);
						lastderror = d_error;

						rx_state = RX_DATA_RECEIVING_STATE;


						// DATA remarked to display BLOCKS n
				//		sprintf_s( m_state_text,"DATA");

						state_text();

                    //	char text[80];

                    //	strftime(ts, 50, "%b-%d-%Y %X", &timeinfo);
                    //	sprintf_s(text, "%s: Receiving Data\r\n", ts);
                    //	func_status(TEXT_STATUS, text);
                    //	sprintf_s(text, "%s: Data state freq. error: (%3.1f hz.)\r\n", ts, d_error);
                    //	func_status(TEXT_STATUS, text);

						if (isnan(datasnr) || isinf(datasnr))
						{
							float dummysnr = 0.0;
                        //	sprintf_s(text, "%s: Data state start SNR: (%3.0f db)\r\n", ts, dummysnr);
                        //	func_status(TEXT_STATUS, text);
						}
						else
						{
                        //	sprintf_s(text, "%s: Data state start SNR: (%3.0f db)\r\n", ts, datasnr);
                        //	func_status(TEXT_STATUS, text);
						}


						training_dcd_reset();

						// Added to trap False Detecion events
						//if( rx_preamble_state_count != 22 )

						// We need to treat 4800bps differently as there is
						// just 1 PREAM state.
						if( rx_mode == M4800S || rx_mode == M75NS || rx_mode == M150S || rx_mode == M300S || rx_mode == M600S || rx_mode == M600V || rx_mode == M1200S || rx_mode == M1200V || rx_mode == M2400S || rx_mode == M2400V  )
						{
							rx_preamble_state_count_test = 1;
						}
						else
						{
							rx_preamble_state_count_test = 19; //22
						}

						//if( rx_preamble_state_count != rx_preamble_state_count_test )
						if( rx_preamble_state_count < rx_preamble_state_count_test )
						{

							if (isnan(preamsnr) || isinf(preamsnr))
							{
								float dummysnr = 0.0;
                            //	sprintf_s(text, "%s: Preamble Fail SNR: (%3.0f db)\r\n", ts, dummysnr);
                            //	func_status(TEXT_STATUS, text);
							}
							else
							{
                        //		sprintf_s(text, "%s: Preamble Fail SNR: (%3.0f db)\r\n", ts, preamsnr);
                            //	func_status(TEXT_STATUS, text);
							}

                        //	sprintf_s(text,"%s: Preamble Fail: Modem Reset\r\n",ts);
                        //	func_status(TEXT_STATUS,text);

							rx_reset();
						}

                        //	sprintf_s(text,"%s: LAST PREAM%i\r\n",ts,rx_preamble_down_count);
							// Send to MS-DMT Scrolling Status Window 
                        //	func_status(TEXT_STATUS,text);

						//if( rx_preamble_state_count != rx_preamble_state_count_test )
						if( rx_preamble_state_count < rx_preamble_state_count_test )
						{
                        //	sprintf_s(text,"%s: PREAM STATE COUNT = %i\r\n",ts,rx_preamble_state_count+2);
							// Send to MS-DMT Scrolling Status Window 
                        //	func_status(TEXT_STATUS,text);
						}
						else
						{
                        //	sprintf_s(text,"%s: PREAM STATE COUNT = %i\r\n",ts,rx_preamble_state_count+2);
							// Send to MS-DMT Scrolling Status Window 
                        //	func_status(TEXT_STATUS,text);
						}


							rx_preamble_state_count = 0;

					}
				}


			}
			break;
		case RX_DATA_RECEIVING_STATE:

			//BIG BOY

			//sprintf_s(text,"%s: DATA Delta Error = %3.8fhz \r\n",ts,delta_error);
			//func_status(TEXT_STATUS,text);

			remove_frequency_error( &b2[M1_RX_BLOCK_LENGTH], delta_error, M1_RX_BLOCK_LENGTH/2 ); // Charles

			//float d_error;
			//d_error        = (float)(-delta_error*2400.0/PI);
			//func_status( FREQ_ERROR_STATUS, &d_error );

			//TEST
			// SNR Display during DATA

			//Update SNR for display in db
			float snr;
			snr = (float) 10.0*(log10(250*(float)cos(acc)*(sample/2))); // works decent - not great accuracy.

			//snr = (float) 20.0*(log10(250*(float)cos(acc)*sample)); // works decent - not great accuracy.
			//snr = 20.0*(log10(100*(float)(acc)*sample));
			//snr = 20.0*(log10(100.0*(float(sample))));

			//snr = ((snr+lastdatasnr)/4);
			if (isnan(preamsnr) || isinf(preamsnr))
			{
				func_status(SNR_STATUS, &lastdatasnr);
			}
			else
			{
				func_status(SNR_STATUS, &snr); //DATA
			}
			datasnr = snr;
			lastdatasnr = snr;

			// ADDED TEST for Freq. Error during decoding
			if (tx_state == TX_IDLE_STATE)
			{
				float d_error;
				static float  delta_error;
				float f_offset;
				f_offset = (float)((50.0 * PI) / (2400.0));
				
				//delta_error = calculate_frequency_error(&b2[M1_RX_BLOCK_LENGTH]) - f_offset;

				delta_error = calculate_frequency_error(&b2[M1_RX_BLOCK_LENGTH]) ;

				//delta_error = calculate_frequency_error(&b2[16]) * f_offset;

				d_error = (float)(-delta_error * 2400.0 / PI);
				d_error = d_error / 10;
				d_error = d_error + lastderror; // Trick, will not track real channel changes
				func_status(FREQ_ERROR_STATUS, &d_error);
			}


			demodulate_block( &b2[get_frame_start()] );

			break;	
		default:
			break;
	}

	state_text();



}
//
// Globally visible interface.
//
/*
void Cm110s::rx_callback( unsigned short *in, int length )
{
	static float sample_block[M1_RX_BLOCK_LENGTH];
	static int   sample_count;

	if( m_rx_enable == false ) return;

	for( int i = 0; i < length; i++ )
	{
		sample_block[sample_count++] = twos_to_float( in[i] );

		if( sample_count == M1_RX_BLOCK_LENGTH )
		{
			process_receive_block( sample_block );
			sample_count = 0;
		}
	}
}
*/
void Cm110s::rx_process_block( signed short *in, int length )
{
    //qDebug()<<Q_FUNC_INFO<<"Entered";
    // return;
    for( int i = 0; i < length; i++ )
	{
        //rx_block_buffer[rx_block_buffer_index++] = twos_to_float( in[i] );
        rx_block_buffer[rx_block_buffer_index++] = (float)(in[i]*0.000030517578125);
		if( rx_block_buffer_index == M1_RX_BLOCK_LENGTH )
		{
			process_receive_block( rx_block_buffer );
			rx_block_buffer_index = 0;

		}
	}
}
bool  Cm110s::rx_is_receiving( void )
{
	if( rx_state == RX_HUNTING_PREAMBLE_STATE )
		return false;
	else
		return true;
}
void Cm110s::set_psk_carrier( long carrier )
{
	psk_carrier_select = carrier;
}
void Cm110s::set_preamble_hunt_squelch( float squelch )
{
	pmag_sql_factor = squelch;

	if(squelch == 8) false_detect_factor = 6;

	if(squelch == 9) false_detect_factor = 5;

	if(squelch == 10) false_detect_factor = 4;

}
void Cm110s::set_noise_reduction_state( int nr )
{
	
	if(nr == 0) nr_state = 0; // Original PMAG VERIFY

	if(nr == 1) nr_state = 1;

}
void Cm110s::set_agc_state( int agc )
{

	if(agc == 0) agc_state = 0;

	if(agc == 1) agc_state = 1;

	if(agc == 2) agc_state = 2;

}
int Cm110s::get_agc_state()
{
	return agc_state;
}
void Cm110s::set_view8psk( int view8psk )
{
	if(view8psk == 0) view8psk_state = 0;

	if(view8psk == 1) view8psk_state = 1;

}
void Cm110s::set_p_mode( int pmode )
{
	if(pmode == 0) pmode_state = 0;

	if(pmode == 1) pmode_state = 1;
}
void Cm110s::set_e_mode( int emode )
{
	if(emode == 0) emode_state = 0;

	if(emode == 1) emode_state = 1;
}
void Cm110s::set_b_mode( int bmode )
{
	if(bmode == 0) bmode_state = 0;

	if(bmode == 1) bmode_state = 1;
}
void Cm110s::set_k_mode( int kmode )
{
	if(kmode == 0) kmode_state = 0;

	if(kmode == 1) kmode_state = 1;
}
void Cm110s::state_text( void )
{
	// RX States = RX_HUNTING_PREAMBLE_STATE,RX_PREAMBLE_FOUND_STATE,RX_DATA_RECEIVING_STATE
/*
	if( rx_state == RX_HUNTING_PREAMBLE_STATE )
		sprintf_s( m_state_text,"IDLE");
	if( rx_state == RX_PREAMBLE_FOUND_STATE )
		sprintf_s( m_state_text,"DATA");
	if( rx_state == RX_DATA_RECEIVING_STATE )
		sprintf_s( m_state_text,"NO EOM");	//IDLE
*/

	if( rx_state == RX_HUNTING_PREAMBLE_STATE )
        sprintf( m_state_text,"IDLE");
	
}
const char* Cm110s::get_state_text( )
{
	return m_state_text;
}
