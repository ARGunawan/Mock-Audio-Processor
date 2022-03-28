#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "AudioProcessor.h"


//Compress 
//This will compress our values so that it will return the compressed version of our function.
std::vector<short> AudioProcessor::Compress(const std::vector<short> & audio, short threshold, float rate)
{
    std::vector<short> Compressed; //Vector to push back the values for our parameter vector audio.
    float Compression_Rate; //This float variable will hold our formula to computer which values need to be compressed 

    //This function will account for the empty vector
     if(audio.size() == 0)
    {
        return audio;
    }

    //Create our for loop to increment through our parameter vector
    for(int i = 0; i < audio.size(); i++)
    {
        //if our threshold is less than or equal to 0 or our rate is less than 1 throw the conditions out
        //As they are out of bounds
        if(threshold <= 0 || rate < 1 )
        {
            throw std::invalid_argument("Invalid Argument!");
        }
        //Same reasoning as the above
        if(threshold == 0 || rate == 0)
        {
            throw std::invalid_argument("Invalid Argument");
        }

        //If in fact there is a threshold and the rate is above 1
        if(threshold >= 0 || rate >= 1)
        {
            //Our Compression Rate forumla is held in a float
            //This formula forms the basis for if the audio is larger than the threshold 
            //Example: (((25 - 10)/1.5) + 10) = 20
            Compression_Rate = (((audio[i] - threshold)/rate) + threshold);

            //Checks if the value in the vector is less than threshold and is greater than the negative counterpart of threshold
            //If condition is met, then push back as is because there is no need for it to be compressed
            if(audio[i] <= threshold && audio[i] >= -threshold)
            {
                Compressed.push_back(audio[i]);
            }

            //Otherwise if our value at that index is a positive number
            else if(audio[i] >= 1)
            {
                //We use 0.5 as our base case for each conditional because we are comparing a float with a short
                //Therefore, we will be converting it this way when we are pushing the rounded version within the condition
                //Example: if 20 >= 20.5, then go to our else statement, gets rounded down to 20
                //Example: if 20.8 >= 20.5, this is true, go to our if statement and rounded up to 21
                if(Compression_Rate >= (int)Compression_Rate + 0.5 ) 
                {
                    Compressed.push_back((int)Compression_Rate + 1); 
                }
                else 
                {
                    Compressed.push_back((int)Compression_Rate);
                }
                
            }

            //Otherwise if our value at the index is a negative value
             else if(audio[i] <= 0)
            {
                //We must take our previous formula and use the absolute value version
                //This is because we need to take account for the fact that we will be dealing with possibly negative numbers
                //If we do not use the absolute value, this will be outputting the wrong values

                Compression_Rate = (((abs(audio[i]) - threshold)/rate) + threshold); 

                //If the value from Compression Rate is greater than its floating counterpart
                //Round it up for the negative number by subtracting. Otherwise just push it out as is if it does not meet this conditon
                if(Compression_Rate >= (int)Compression_Rate + 0.5) 
                {
                    Compressed.push_back(-(int)Compression_Rate - 1);
                }
                else
                {
                    Compressed.push_back(-(int)Compression_Rate);
                }
            }  
        }
    }
    //Return our vector of shorts
    return Compressed;
}

//CutOutSilence
//This will remove values in our vector within the range of our levels ([-level, level]) depending on the length that needs to be removed
std::vector<short> AudioProcessor::CutOutSilence(const std::vector<short> & audio, short level, int silenceLength)
{
    std::vector<short> Silence; //Our vector to push our values into 
    int Counter = 0;            //Initialize an int variable to store the number of values that meet the criteria to be removed 
    int SL = 0;                 //This will be used to check the length of Counter and if it meets our length to be removed

    //Again we have to check for the empty vector
    if(audio.size() == 0)
    {
        return audio;
    }
    //If level and silence length are out of bouds, just throw it away
    //They will never be compiled into our program since they do not meet the minimum requirement
    if (level < 0 || silenceLength < 1)
    {
        throw std::invalid_argument("Invalid Argument!");
    }

    //Otherwise if they do meet the requirements
    else if (level > 0 || silenceLength > 1)
    {
        //Create our for loop to increment through our vector
        for(int i = 0; i < audio.size(); i++)
        {
            //If the values are withing the range, then increment the counter to for the values
            //Increment the SL as well that stores the length of silence length to check for the length
            if ((audio[i] >= (-level)) && (audio[i] <= level))
            {
                SL++;
                Counter++;
            }

            //Otherwise if the above condition is not met
            //If our stored length is less than the actual length that is enabled to cut
            else 
            { 
                if(SL < silenceLength) 
                {
                    //While the counter is larger than 0
                    //Pushback our vector silence. We subtract the index with counter to prevent going out of bounds
                    //Decrement by one to prevent being out of bounds again 
                    while(Counter > 0)
                    {
                        Silence.push_back(audio[i - Counter]);
                        Counter--;
                    }
                }
                //These will clear the stored variables for SL and Counter to reset them
                //We then push back the vector
                SL = 0;
                Counter = 0;
                Silence.push_back(audio[i]);
            }
        }
        //If our required length to remove the values is larger than the ones that our variable is holding
        if(SL < silenceLength)
        {
            //While the Counter is larger than 0
            //Push back the vector and subtract it once more to prevent being out of bounds
            //Decrement once more for our counter
            while(Counter > 0)
            {
                Silence.push_back(audio[audio.size() - Counter]);
                Counter--;
            }
        }
    }
    //return the values for our vector
    return Silence; 
}

//StretchTwice
//This will take the average of the audio samples and return the audio where an extra audiosample in between appears
std::vector<short> AudioProcessor::StretchTwice(const std::vector<short> & audio)
{
    std::vector<short> Stretched; //Again create a vector 
    float Average_of_Two;         //Create a floating point variable that will hold the math to take the average

    //If the vector only contains one element return it
    if(audio.size() == 1)
    {
        return audio;
    }
    //If the vector has nothing in there, just return it as it
    if(audio.size() == 0)
    {
        return audio;
    }

//Our for loop to check the incrementation
//We make sure to do audio.size() - 1, to prevent the last value from being out of bounds
for(int i = 0; i < audio.size() - 1;i++)
{
    //We set floating point values for the samples because our Average is possibly going to deal with floating point values
    //Therefore we must account for this possibility by intializing the values max and min
    //The max will ensure that we are getting a value that is largest and min will ensure we get the smallest
    //This will prevent make sure that we are adding the next value with each other because audio[i] could potentially be larger or smaller than audio[i+1]
    float first_sample =  audio[i];
    float second_sample =  audio[i+1];
    float max = std::max(first_sample,second_sample);
    float min = std::min(first_sample,second_sample);
    
    //Our formula to hold the average. Very standard form of taking the average
    Average_of_Two = (max + min)/2;

    //If i is equal to our audio size just push and round it up 
    if(i == audio.size() - 1)
    {
        Stretched.push_back(audio[i + 1]); 
    }
    //Otherwise if that is not the case
    //If our index of our parameter vector is less than the index after it
    //Create several conditions to compare the shorts and floating point. Then push the values afterwards and round down or up.
    else if(audio[i] <= audio[i+1])
    {   
        //If the average is greater than the floating point
        // Push back the audio and then round it up by one
        if(Average_of_Two >= (int)Average_of_Two + 0.5)
        {
            Stretched.push_back(audio[i]);
            Stretched.push_back((int)Average_of_Two + 1);   
        }
        else
        {   
            //If the average is greater than 0
            //Push the code as is 
            if(Average_of_Two > 0)
            {   
                Stretched.push_back(audio[i]);
                Stretched.push_back((int)Average_of_Two);   
            }

            //else If the average is greater than the floating point
            // Push back the audio and then round it down by one
            else if(Average_of_Two >= (int)Average_of_Two + 0.5)  
            {
                Stretched.push_back(audio[i]);
                Stretched.push_back((int)Average_of_Two - 1);
            }

            //else If the averages do not meet the conditions
            else
            {
                //if the negative average is greater than the floating point of its negative counterpart
                //Push back the audio 
                if(-Average_of_Two < -(int)Average_of_Two + 0.5)
                {
                   Stretched.push_back(audio[i]);
                   Stretched.push_back((int)Average_of_Two);
                }
                //else 
                //Push back the audio and then round it down by one
                else 
                {
                   Stretched.push_back(audio[i]);
                   Stretched.push_back((int)Average_of_Two - 1);

                }
            }
        }
    } 

//Else if the conditions above do not match
else
{
    //if the average is greater than its floating point (either negative or positive).
    // Push back the audio and then round it up by one
    if((Average_of_Two >= (int)Average_of_Two + 0.5) && (-Average_of_Two >= -(int)Average_of_Two + 0.5))
    {
        Stretched.push_back(audio[i]);
        Stretched.push_back((int)Average_of_Two + 1);
    }

    //else if the conditions above do not match 
    //Create more conditions 
    else
    {
        //If the average is greater than the floating point
        // Push back the audio and then round it up by one
        if((Average_of_Two) >= ((int)Average_of_Two + 0.5))
        {
               
            Stretched.push_back(audio[i]);
            Stretched.push_back((int)Average_of_Two  + 1);
        }
        else 
        {
            //If the negative average is greater than the floating point counterpart
            // Push back the audio as it
            if(-(Average_of_Two) < (-(int)Average_of_Two + 0.5))
            {   
                Stretched.push_back(audio[i]);
                Stretched.push_back((int)Average_of_Two);
            }

            //else If the average is greater than the floating point
            // Push back the audio and then round it down by one
            else
            {
                    Stretched.push_back(audio[i]); 
                    Stretched.push_back((int)Average_of_Two - 1);
            }
        }
    }  
    } 
}
    //return our vector and push back the audio size by one to prevent it from going out of bounds
    Stretched.push_back(audio[audio.size() - 1]);
    return Stretched;
}  

//Normalize
//This will return a sample where the audio are all increased or decreased proportionally that ensures that
//the largest absolute value is the same as normalizeTarget
std::vector<short> AudioProcessor::Normalize(const std::vector<short> &audio, short normalizeTarget)
{
    float Largest_Value = abs(audio[0]); //Floating point to hold our largest value
    std::vector<short> lv;               //Vector to hold the values
    float Normal;                        //Normal is where we are storing our formulas

    //if the parameter vector is empty, just return the vector parameter
    if(audio.size() == 0)
    {
        return audio;
    }

    //Use a for loop to run through our parameter vector
    for(int i = 0; i < audio.size();i++)
    {
        //If normalize target does not meet the required standards just throw it out
        if(normalizeTarget < 1 || normalizeTarget == 0)
        {
            throw std::invalid_argument("Invalid Argument!");
        }
        //If normalize target does meet the required standards
        //Then if our absolute value for the index is larger the largest value at the index abs(audio[0])
        if(normalizeTarget > 1)
        {
            //if current position is greater than 0
            //Largest value is equal to the absolute value of audio index
            if(abs(audio[i]) > Largest_Value) 
            {
                Largest_Value = abs(audio[i]);
            }
        }
        //Otherwise if none of the above conditions meet, just throw it 
        else
        {
            throw std::invalid_argument("Invalid Argument!");
        }
    }

    //Create another for loop to increment for our actual arithmetic
    for(int i = 0; i < audio.size();i++)
    {
        //If normalize target does not meet the required standards just throw it out
        if(normalizeTarget < 1 || normalizeTarget == 0)
        {
            throw std::invalid_argument("Invalid Argument");
        }
        //If normalize target does meet the required standards
        //Then if our absolute value for the index is larger the largest value at the index abs(audio[0])
        if(normalizeTarget > 1)
        {
            //Normal is where we are storing our value for the math
            //Divide normalize targer with the largest value, then multiply that value to the rest of the vector
            //This will make sure everything is either increased or decreased
            Normal = (normalizeTarget/Largest_Value) * abs(audio[i]);
            
            //If the parameter vector is larger than 0
            if(audio[i] > 0)
            {
                //If Normal is larger or equal to the floating pointer counterpart
                //Push back the vector of lv and round it up 
                if(Normal >= (int)Normal + 0.5)
                {
                    lv.push_back((int)Normal + 1); 
                }
                //Else push back the vector of lv 
                else 
                {
                lv.push_back((int)Normal); 
                }
            }
        else
        {
            //If Normal is larger or equal to the floating pointer counterpart
            //Push back the vector of lv as a negative and round it down 
            if(Normal >= (int)Normal + 0.5)
            {
                lv.push_back(-(int)Normal - 1); 
            }
            //Else push back the vector of lv as a negative 
            else 
            {
                lv.push_back(-(int)Normal); 
            }
        }
        }
    } 
    //return the vector
    return lv;
}