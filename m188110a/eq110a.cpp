/*
 * 
 * 110A modem equalizer.
 *
 */
#include <math.h>
#include "Cm110s.h"

void Cm110s::kalman_reset_coffs(void)
{
	int i;
	
	for( i=0; i < M1_KN ; i++ )
	{
		c[i].re = 0.0;
		c[i].im = 0.0;
	}
}
void Cm110s::kalman_reset_ud( void )
{
	int i,j;

	for( j = 0 ; j < M1_KN ; j++ )
	{
		for( i = 0; i < j ; i++ )
		{
			u[i][j].re = 0.0;
			u[i][j].im = 0.0;
		}
		d[j]         = 1.0;
	}
}

/*
 *
 *

 * Reset Kalman variables, to ensure stability 
 *
 *
 */
void Cm110s::kalman_reset( void )
{
	kalman_reset_ud();
	kalman_reset_coffs();
}
/*
 *
 *
 * Initialise this module
 *
 *
 */
void Cm110s::kalman_init( void )
{
	//q      = 0.008f; //Charles

	q      = 0.007f;   //<<<<<<<<<<<<<<<<< BEST PERIOD

	//BIG CHILD
	//q      = 0.006f; // Best so far

	// =======================================

	//E      = 0.001f; // Charles
	// ADDED TEST
	//E      = 0.03f;  // From CadetALE

	E        = 0.008f;  //BIG CHILD      // <<<<<<<< VERY BEST so far

	//BIG CHILD
	//E        = 0.01f;  //Best so far
	//E        = 0.009f;  //Same

	kalman_reset_ud();
	kalman_reset_coffs();
}
/*
 *
 *
 * Modified Root Kalman gain Vector estimator
 *
 *
 */
void Cm110s::kalman_calculate( FComplex *x )
{
	int        i,j;
	FComplex   B0;
	float      hq;
	float      B;
	float      ht;

   	f[0].re =  x[0].re;               // 6.2
	f[0].im = -x[0].im;

	for( j = 1; j < M1_KN ; j++)              // 6.3
	{
		f[j].re  = cmultRealConj(u[0][j],x[0]) + x[j].re; 
		f[j].im  = cmultImagConj(u[0][j],x[0]) - x[j].im;

		for( i = 1 ; i < j ; i++ )
		{			
			f[j].re += cmultRealConj(u[i][j],x[i]);
			f[j].im += cmultImagConj(u[i][j],x[i]); 
		}
	}
	for( j = 0; j < M1_KN ; j++)                // 6.4
	{
		g[j].re = d[j]*f[j].re;
		g[j].im = d[j]*f[j].im;
	}
    a[0] = E + cmultRealConj(g[0],f[0]); 	 // 6.5

	for( j = 1; j < M1_KN ; j++ ) // 6.6
	{
		a[j] = a[j-1] + cmultRealConj(g[j],f[j]);
	}

	hq  = 1 + q;                              // 6.7
	ht  = a[M1_KN-1]*q;

	y = 1.0f/(a[0]+ht);                       // 6.19

	d[0] = d[0] * hq * ( E + ht ) * y;       // 6.20

	// 6.10 - 6.16 (Calculate recursively)

	for( j = 1; j < M1_KN ; j++ )
	{
		B = a[j-1] + ht;                 // 6.21
		h[j].re = -f[j].re*y;        // 6.11
		h[j].im = -f[j].im*y;

		y = 1.0f/(a[j]+ht);               // 6.22

		d[j] = d[j]*hq*B*y;              // 6.13
		
 		for( i = 0; i < j ; i++ )
		{

			B0           =  u[i][j];
			u[i][j].re =  B0.re + cmultRealConj(h[j],g[i]); // 6.15
			u[i][j].im =  B0.im + cmultImagConj(h[j],g[i]);

			g[i].re +=  cmultRealConj(g[j],B0);               // 6.16
			g[i].im +=  cmultImagConj(g[j],B0);
		}
	}
}
/*
 *
 * Update the filter coefficients using the Kalman gain vector and 
 * the error
 *
 */

void Cm110s::kalman_update( FComplex *data, FComplex error )
{
	int i;
	//
  	// Calculate the new Kalman gain vector 
	//

	kalman_calculate( data );
	//
	// Update the filter coefficients using the gain vector
	// and the error.
	//

	error.re *= y;
	error.im *= y;

	for( i = 0; i < M1_KN ; i++ )
	{
		c[i].re  += cmultReal(error,g[i]);
		c[i].im  += cmultImag(error,g[i]);
	}
}

FComplex Cm110s::equalize( FComplex *in )
{
	int      i;
	FComplex symbol;
	
	for( i = 0 ; i < M1_FF_EQ_LENGTH ; i++ )
	{
		d_eq[i] = in[i];
	}
	/* Calculate the symbol */ 
	
	symbol.re = cmultReal(d_eq[0],c[0]);
	symbol.im = cmultImag(d_eq[0],c[0]);

	for( i=1; i < M1_KN; i++ )
	{
		symbol.re += cmultReal(d_eq[i],c[i]);
		symbol.im += cmultImag(d_eq[i],c[i]);
	}
	
	return symbol;
}
/*
 * Initialize the equalizer.
 */
void Cm110s::equalize_init(void)
{
	kalman_init();
}
/*
void Cm110s::update_fb( FComplex in )
{
	int i;

	for( i = (M1_KN-1) ; i > M1_FF_EQ_LENGTH ; i-- )
	{
		d_eq[i] = d_eq[i-1];
	}
	d_eq[M1_FF_EQ_LENGTH] =   in; // Update
}
*/
//
// Started using this on 2 FEB 2012 C5B9
//
void Cm110s::update_fb( FComplex in )
{
	int i;

	for( i =  M1_FF_EQ_LENGTH; i < M1_KN-1; i++ )
	{
		d_eq[i+1] = d_eq[i];
	}
	d_eq[M1_FF_EQ_LENGTH] =   in; //Update 
}
/*
 *
 * Train the equalizer using known symbols
 *
 */
FComplex Cm110s::equalize_train( FComplex *in, FComplex train )
{
	FComplex error;
	FComplex symbol;

//	train.re = train.re*0.01f; //Charles
//	train.im = train.im*0.01f;

	// JULY 2017 this seems to work better when Sweep Rate High
//	train.re = train.re*0.02f;
//	train.im = train.im*0.02f;

	// JULY 2017 this seems to work best!
	train.re = train.re*0.015f;
	train.im = train.im*0.015f;

	symbol = equalize( in );	
	/* Calculate error */
	error.re =  train.re - symbol.re;
	error.im =  train.im - symbol.im;	

	/* START: Do we need this? */

	// Update the DCD training seq information
	//if(( symbol.re * train.re ) < 0 ) m_train_errors++;  //Charles
	if(( symbol.re * train.re ) < 0 ) m_train_errors++;

	m_train_count++;

	/* END: Do we need this? */	

	/* Update the coefficients */
	kalman_update(d_eq,error);         
	
	/* Update the quality mse */
	update_mse_average_ber(error);



	// ADDED TEST

	// Train the equalizer using KNOWN symbols
	// Dislay Constellation Symbols in 8PSK during


	// Need to code to ENABLE/DISABLE via MMI for Engineering use
if((emode_state == 1 || bmode_state == 1) && kmode_state == 0)
{
	switch( rx_mode )
	{
		case M75NS:
			//FComplex    consym;  // <<= Needed if enabled
			//consym.re = symbol.re *125;
			//consym.im = symbol.im *125;
			//rx_update_pos_eq_constellation( consym );
			break;
		case M75NL:
			//consym.re = symbol.re *125;
			//consym.im = symbol.im *125;
			//rx_update_pos_eq_constellation( consym );
			break;
		case M150S:
		case M150L:
			FComplex    consym;   // <<= Remark out if 75bps enabled
			consym.re = symbol.re *125;
			consym.im = symbol.im *125;

			rx_update_pos_eq_constellation( consym );

			break;
		case M300S:
		case M300L:

			consym.re = symbol.re *125;
			consym.im = symbol.im *125;

			rx_update_pos_eq_constellation( consym );

			break;
		case M600S:
		case M600L:
		case M600V:

			consym.re = symbol.re *125;
			consym.im = symbol.im *125;

			rx_update_pos_eq_constellation( consym );

			break;
		case M1200S:
		case M1200L:
		case M1200V:

			consym.re = symbol.re *125;
			consym.im = symbol.im *125;

			rx_update_pos_eq_constellation( consym );

			break;
		case M2400S:
		case M2400L:
		case M2400V:

			consym.re = symbol.re *125;
			consym.im = symbol.im *125;

			rx_update_pos_eq_constellation( consym );

			break;
		case M4800S:

			consym.re = symbol.re *125;
			consym.im = symbol.im *125;

			rx_update_pos_eq_constellation( consym );

			break;
		default:
			break;
	}

}

	switch( rx_mode )
	{
		case M75NS:
			break;
		case M75NL:
			break;
		case M150S:
		case M150L:
			break;
		case M300S:
		case M300L:
			break;
		case M600S:
		case M600L:
		case M600V:
			break;
		case M1200S:
		case M1200L:
		case M1200V:
			break;
		case M2400S:
		case M2400L:
		case M2400V:
			//update_mse_average(error);
			break;
		case M4800S:
			update_mse_average(error); // Must for 4800bps 
			break;
		default:
			break;
	}


	/* Update the FB data */
	update_fb( train );

	return(symbol);
}
void Cm110s::equalize_update_initial_fb( FComplex *train, int count )
{
	for( int j = 0; j < count; j++ )
	{
		update_fb(train[j]);
	}
}
FComplex Cm110s::equalize_train_seq( FComplex *in, FComplex scramble, FComplex train )
{

	FComplex temp;
	FComplex symbol;

	temp.re = cmultReal( train, scramble );
	temp.im = cmultImag( train, scramble );

	symbol =  equalize_train( in, temp );

	temp.re = cmultRealConj( symbol, scramble );
	temp.im = cmultImagConj( symbol, scramble );

	// Return the decoded symbol
	return(temp);

}
/*
 *
 * Equalize the data and train on the decision
 *
 */
FDemodulate Cm110s::equalize_data( FComplex *in, FComplex scramble )
{
	FComplex    symbol;
	FDemodulate decision;
	
	symbol = equalize( in );

	/* Decode */
	
	decision = (*this.*demod)( symbol, scramble );

	// ADDED TEST
//	update_mse_average(decision.error);
//  July 2017 removed from use for testing


	

	// 
	// Need to code to ENABLE/DISABLE via MMI for Engineering use.

	// Equalize the data and train on the decision
	// Display 8PSK Constellation Symbols for 

if((emode_state == 1 || bmode_state == 1) && kmode_state == 1)
{
	switch( rx_mode )
	{
		case M75NS:
			break;
		case M75NL:
			break;
		case M150S:
		case M150L:
			FComplex    consym;
			consym.re = symbol.re *125;
			consym.im = symbol.im *125;
			rx_update_pos_eq_constellation( consym );
			break;
		case M300S:
		case M300L:
			consym.re = symbol.re *125;
			consym.im = symbol.im *125;
			rx_update_pos_eq_constellation( consym );
			break;
		case M600S:
		case M600L:
		case M600V:
			consym.re = symbol.re *125;
			consym.im = symbol.im *125;
			rx_update_pos_eq_constellation( consym );
			break;
		case M1200S:
		case M1200L:
		case M1200V:
			consym.re = symbol.re *125;
			consym.im = symbol.im *125;
			rx_update_pos_eq_constellation( consym );
			break;
		case M2400S:
		case M2400L:
		case M2400V:
			update_mse_average(decision.error);
			consym.re = symbol.re *125;
			consym.im = symbol.im *125;
			rx_update_pos_eq_constellation( consym );
			break;
		case M4800S:
			//update_mse_average(decision.error);
			consym.re = symbol.re *125;
			consym.im = symbol.im *125;
			rx_update_pos_eq_constellation( consym );
			break;
		default:
			break;
	}

}

	switch( rx_mode )
	{
		case M75NS:
			break;
		case M75NL:
			break;
		case M150S:
		case M150L:
			break;
		case M300S:
		case M300L:
			break;
		case M600S:
		case M600L:
		case M600V:
			break;
		case M1200S:
		case M1200L:
		case M1200V:
			break;
		case M2400S:
		case M2400L:
		case M2400V:
			update_mse_average(decision.error);
			break;
		case M4800S:
			//update_mse_average(decision.error);
			break;
		default:
			break;
	}



	// Look at this for PSKsounder like Multipath/Fading display

	/* Update the coefficients */
	kalman_update(d_eq,decision.error);
	
	/* Update the FB data */
	update_fb( decision.dx_symbol );


	return decision;
}
/*
 *
 *
 * Use the end of the preamble to train the equalizer.
 *
 *
 */
void Cm110s::train_on_preamble( FComplex *in, Mode mode, int count )
{
	int i, index;

	/* The last few symbols will always be a zero */
	/* Load them into the FB portion              */

	//equalize_update_initial_fb( p_zero, M1_P_ZERO_LENGTH );

	for( int j = 0; j < count; j++ )
	{
		update_fb(in[j]);
	}


	/* Do the common part of the preamble */
	
	for( i = 0, index = 0; i < M1_P_COMMON_LENGTH; i++, index += 2 )
	{
		equalize_train( &in[index], p_common[i] );
	}

	/* Do the rate part of the preamble */
	
	for( i = 0; i < M1_P_MODE_LENGTH; i++, index += 2 )
	{
		equalize_train( &in[index], p_mode[mode][i] );
	}

	/* Do the count part of the preamble */
	
	for( i = 0; i < M1_P_COUNT_LENGTH; i++, index += 2 )
	{
		equalize_train( &in[index], p_count[count][i] );
	}

	/* Do the final zero part of the preamble */
	
	for( i = 0; i < M1_P_ZERO_LENGTH; i++, index += 2 )
	{
		equalize_train( &in[index], p_zero[i] );
	}
}
