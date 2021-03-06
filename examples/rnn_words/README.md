# Names generation example with Recurrent ANN

This example attempts to generate some names of cities – random, yet sounding more or less naturally. For this, a recurrent artificial neural network is trained using a [dataset of US cities](../data/words). Each city name is represented as a sequence of characters and the network is trained to predict next character based on provided current character and the history of previous characters (internal state of the network). Since many of the cities' names in the dataset have contradicting sequences of characters (like "Bo" can be followed by 's' as in "Boston" or by 'u' as in "Boulder", etc), it is unlikely the network will memorize any of the names. Instead it should pick common most frequent patterns of characters' transitions. Once the network is trained (certain number of epochs) it is used to generate new names. The network is presented with one or more random characters to start with and then its output is used to complete the new generated city name.

Each character of a word sequence is one-hot encoded - 30 characters/labels are used: 26 for 'A' to 'Z', 3 for '.', '-' and space, 1 for string terminator. As the result, the neural network has 30 inputs and 30 outputs. The first layer is GRU (gated recurrent unit) and the second layer is fully connected.

```C++
// prepare a recurrent ANN
shared_ptr<XNeuralNetwork> net = make_shared<XNeuralNetwork>( );

net->AddLayer( make_shared<XGRULayer>( LABELS_COUNT, 60 ) );
net->AddLayer( make_shared<XFullyConnectedLayer>( 60, LABELS_COUNT ) );
net->AddLayer( make_shared<XSoftMaxActivation>( ) );
```

The helper ExtractSamplesAsSequence() function takes care of converting vocabulary words into training sequences. For example, if the word to encode is "BOSTON", then it will generate the next training sequence (one-hot encoding is applied on top of it though):

|               | 0 | 1 | 2 | 3 | 4 | 5    | ... |
| ------------- | - | - | - | - | - | ---- | --- |
| Input         | B | O | S | T | O | N    | ... |
| Target output | O | S | T | O | N | null | ... |

Since this example application uses batch training, each training sequence must be of the same length, which is the length of the longest word in the training vocabulary. As the result, many of the training sequences will be padded with string terminator.

```C++
// create training context with Adam optimizer and Cross Entropy cost function
XNetworkTraining netTraining( net,
                              make_shared<XAdamOptimizer>( LEARNING_RATE ),
                              make_shared<XCrossEntropyCost>( ) );

netTraining.SetAverageWeightGradients( false );
netTraining.SetTrainingSequenceLength( maxWordLength ); /* sequence length as per the longest word */

vector<fvector_t> inputs;
vector<fvector_t> outputs;

for ( size_t epoch = 0; epoch < EPOCHS_COUNT; epoch++ )
{
    // shuffle training samples
    for ( size_t i = 0; i < samplesCount / 2; i++ )
    {
        int swapIndex1 = rand( ) % samplesCount;
        int swapIndex2 = rand( ) % samplesCount;

        std::swap( trainingWords[swapIndex1], trainingWords[swapIndex2] );
    }

    for ( size_t iteration = 0; iteration < iterationsPerEpoch; iteration++ )
    {
        // prepare batch inputs and ouputs
        ExtractSamplesAsSequence( trainingWords, inputs, outputs, BATCH_SIZE,
                                  iteration * BATCH_SIZE, maxWordLength );

        auto batchCost = netTraining.TrainBatch( inputs, outputs );
        netTraining.ResetState( );
    }
}
```

# Examples of generated names

To demonstrate that trained network can generate something interesting, lets first have a look at some words generate by untrained network:
* Uei-Dkpcwfiffssiafssvss
* Ajp
* Vss
* Oqot
* Mx-Ueom Lxuei-Kei-T
* Qotbbfss
* Ei-Mfkx-Ues
* Wfsssa

And here is a small list of some more interesting names of cities generated by a trained neural network. Yes, may sound unusual. But still better than “asdf”.
* Bontoton
* Mantohantot
* Deranber
* Contoton
* Jontoron
* Gantobon
* Urereton
* Rantomon
* Mantomon
* Zontolen
* Zontobon
* Lentohantok
* Tontoton
* Lentomon
* Xintox
* Contovillen
* Wantobon
* Intoncon
