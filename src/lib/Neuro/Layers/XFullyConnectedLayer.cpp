/*
    ANNT - Artificial Neural Networks C++ library

    Copyright (C) 2018, cvsandbox, cvsandbox@gmail.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "XFullyConnectedLayer.hpp"

using namespace std;

namespace ANNT { namespace Neuro {

XFullyConnectedLayer::XFullyConnectedLayer( size_t inputsCount, size_t outputsCount ) :
    ITrainableLayer( inputsCount, outputsCount ),
    mWeights( inputsCount * outputsCount ),
    mBiases( outputsCount )
{
    Randomize( );
}

// Randomizes layer's weights, clears biases
void XFullyConnectedLayer::Randomize( )
{
    float_t halfRange = sqrt( float_t( 3 ) / mInputsCount );

    for ( auto& w : mWeights )
    {
        w = ( static_cast<float_t>( rand( ) ) / RAND_MAX ) * ( float_t( 2 ) * halfRange ) - halfRange;
    }
    for ( auto& b : mBiases )
    {
        b = 0;
    }
}

// Calculates outputs for the given inputs
void XFullyConnectedLayer::ForwardCompute( const vector<fvector_t*>& inputs,
                                           vector<fvector_t*>& outputs )
{
    for ( size_t i = 0, n = inputs.size( ); i < n; i++ )
    {
        const fvector_t& input  = *( inputs [i] );
        fvector_t&       output = *( outputs[i] );
        size_t           weightIndex = 0;

        for ( size_t otputIndex = 0; otputIndex < mOutputsCount; otputIndex++ )
        {
            float_t sum = 0;

            for ( size_t inputIndex = 0; inputIndex < mInputsCount; inputIndex++, weightIndex++ )
            {
                sum += input[inputIndex] * mWeights[weightIndex];
            }

            output[otputIndex] = sum + mBiases[otputIndex];
        }
    }
}

// Propagates error to the previous layer and calculates weights/biases gradients
void XFullyConnectedLayer::BackwardCompute( const vector<fvector_t*>& inputs,
                                            const vector<fvector_t*>& /* outputs */,
                                            const vector<fvector_t*>& deltas,
                                            vector<fvector_t*>& prevDeltas,
                                            fvector_t& gradWeights,
                                            fvector_t& gradBiases )
{
    // 1 - first propagate deltas to the previous layer
    for ( size_t i = 0, n = inputs.size( ); i < n; i++ )
    {
        fvector_t&       prevDelta = *( prevDeltas[i] );
        const fvector_t& delta     = *( deltas[i] );

        for ( size_t inputIndex = 0; inputIndex < mInputsCount; inputIndex++ )
        {
            size_t  weightIndex = inputIndex;
            float_t sum         = 0;

            for ( size_t otputIndex = 0; otputIndex < mOutputsCount; otputIndex++, weightIndex += mInputsCount )
            {
                sum += delta[otputIndex] * mWeights[weightIndex];
            }

            prevDelta[inputIndex] = sum;
        }
    }

    // 2 - accumulate weights' difference
    for ( size_t outputIndex = 0, weightIndexStart = 0; outputIndex < mOutputsCount; outputIndex++, weightIndexStart += mInputsCount )
    {
        for ( size_t i = 0, n = inputs.size( ); i < n; i++ )
        {
            const fvector_t& input      = *( inputs[i] );
            float_t          deltaValue = ( *( deltas[i] ) )[outputIndex];

            for ( size_t inputIndex = 0, weightIndex = weightIndexStart; inputIndex < mInputsCount; inputIndex++, weightIndex++ )
            {
                gradWeights[weightIndex] += deltaValue * input[inputIndex];
            }
        }
    }

    // 3 - accumulate baises' difference
    for ( size_t i = 0, n = inputs.size( ); i < n; i++ )
    {
        const fvector_t& delta = *( deltas[i] );

        for ( size_t outputIndex = 0; outputIndex < mOutputsCount; outputIndex++ )
        {
            gradBiases[outputIndex] += delta[outputIndex];
        }
    }
}

// Applies updates to the layer's weights and biases
void XFullyConnectedLayer::UpdateWeights( const fvector_t& weightsUpdate,
                                          const fvector_t& biasesUpdate )
{
    for ( size_t i = 0, n = mWeights.size( ); i < n; i++ )
    {
        mWeights[i] += weightsUpdate[i];
    }
    for ( size_t i = 0, n = mBiases.size( ); i < n; i++ )
    {
        mBiases[i] += biasesUpdate[i];
    }
}

} } // namespace ANNT::Neuro
