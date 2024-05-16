// Jude
// Feburary 11, 2024
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <numeric>


const std::vector<int> comLettInEng {4, 19, 0, 14, 8, 13, 18, 7, 17, 3}; //Ten of the most common letters in English
                                    //E  T  A   O  I   N   S  H   R  D
std::vector<char> alphabet(26);
std::vector<int> keyShift (100);

//Calculates the frequency of letters in a given string
std::vector<int> freqGenerator (std::string a){

    std::vector<int> frequency(26);

    for (int i = 0; i < a.length(); i++){
        for (int j = 0; j < alphabet.size(); j++){
            if(a[i] == alphabet[j]) frequency[j] += 1;
        }
    }
    return frequency;
}

//Calculates the Index of Coincidence for a given string
double iocGenerator (std::string a){

    double ioc = 0;
    std::vector<int> freqOfA = freqGenerator(a);

    for (int i = 0; i < alphabet.size(); i++){
        ioc += ((double)freqOfA[i]/a.size()) * (((double)freqOfA[i]-1)/(a.size()-1));
    }

    return ioc;
}

//decrypts a given string
std::string decryptor (int largest, std::vector<std::string> key, int index, int pass){

    std::string decrypted = key[index];

    keyShift[index] = largest - comLettInEng[pass];

    for(int i = 0; i < key[index].length(); i++){
        for(int j = 0; j < alphabet.size(); j++){
            if (alphabet[j] == key[index][i])
            {
                if ((j - keyShift[index]) >= 0) decrypted[i] = alphabet[j - keyShift[index]]; 
                else decrypted[i] = alphabet[(j - keyShift[index]) + 26];
            }
        }
    }
    
    return decrypted;
}

int main()
{
    //Initialize the alphabet A-Z
   std::iota(alphabet.begin(), alphabet.end(), 'A');

   std::ifstream f("cipherNoKey.txt"); //taking file as inputstream
   std::string encryption;
   if(f) {
      std::ostringstream ss;
      ss << f.rdbuf(); // reading data
      encryption = ss.str();
   }

    //Initialize the guessed lenght of the key, start with 1
    std::vector<std::string> keyGuess(1);
    std::vector<double>      keyIOC  (1);
    std::vector<bool>        check   (1);

    int  lenGuess     = 1;
    bool inRange      = false;
    int  booleanCheck = 1;

    //Guess the length by means of IOC
    while (inRange == false && lenGuess < encryption.length()){
        for (int i = 0; i < encryption.length(); i++) {
            keyGuess[i % keyGuess.size()].push_back(encryption[i]);
        }

        //Generate IOC for each part of the key
        for (int i = 0; i < keyGuess.size(); i++){
            keyIOC[i] = iocGenerator(keyGuess[i]);
        }

        //In order for the string to be English, its range needs to be between 0.06 to 0.09
        for (int i = 0; i < keyGuess.size(); i++){
            if (keyIOC[i] > 0.06 && keyIOC[i] < 0.09) check[i] = true;
            else check[i] = false;
        }

        //Check to see if all values in vector check are true
        for (int i = 0; i < keyGuess.size(); i++){
            booleanCheck *= check[i];
        }

        //booleanCheck will be true if all strings in the vector keyGuess are in English's IOC
        if (booleanCheck == 1) inRange = true;

        else{
            lenGuess++;

            //Reset and resize variables
            keyGuess.clear();
            keyGuess.resize(lenGuess);

            keyIOC.clear();
            keyIOC.resize(lenGuess);

            check.clear();
            check.resize(lenGuess);

            booleanCheck = 1;
        }
    }

    std::vector<std::string> decrypted(lenGuess);
    std::vector<int> larLocat(lenGuess);
    std::string finMessage;

    //Calculate the position of the most common letters in each index
    for (int i = 0; i < lenGuess; i++){
        std::vector<int> lenFreq = freqGenerator(keyGuess[i]);
        int largest = *max_element(lenFreq.begin(), lenFreq.end());
        for (int j = 0; j < alphabet.size(); j++) if (lenFreq[j] == largest) larLocat[i] = j;
    }

    //Decrypt the strings in the keyGuess vector
    for (int i = 0; i < lenGuess; i++){
        decrypted[i] = decryptor(larLocat[i], keyGuess, i, 0);
    }

    for (int i = 0; i < decrypted[0].length(); i++){
        for(int j = 0; j < decrypted.size(); j++){
            finMessage.push_back(decrypted[j][i]);
        }
    }

    std::string key;
    //Generate the estimated key
    for(int i = 0; i < lenGuess; i++){
        if (keyShift[i] < 0) key.push_back(alphabet[keyShift[i]+26]);
        else key.push_back(alphabet[keyShift[i]]);
    }
    char input;
   
    while(true){ 

        std::cout << finMessage << "\n \nThe key length is " << lenGuess;
        std::cout << "\nEstimated key:" << ' ' << key << '\n' << "Is this message comprehensible? \nY/N? "; 

        char input;
        
        std::cin >> input;

        if (input == 'Y') break;
        finMessage.clear();

        std::cout << "\nDo you want to change the common letter of an index (C) or guess the key (G): ";
        std::cin >> input;

        //Recompute an index in the Key
        if (input == 'C'){
            std::cout << "\nIndex of Coincidence of each index in key:\n";
            for (int i = 0; i < lenGuess; i++){
                std::cout << i << ' ' << keyIOC[i] << '\n';
            }

            std::cout << "Which index do you want to change? ";
            int index;
            std::cin >> index;

            std::cout << "Please select the index of the common English letter you want the shift to be by ";
            std::cout << "(NOTE: The default is E)\n";

            for(int i = 0; i < comLettInEng.size(); i++){
                std::cout << i << ' ' << alphabet[comLettInEng[i]] << '\n';
            }
            int pass;
            std::cin >> pass;

            decrypted[index] = decryptor(larLocat[index], keyGuess, index, pass);

            for (int i = 0; i < decrypted[0].length(); i++){
                for(int j = 0; j < decrypted.size(); j++){
                    finMessage.push_back(decrypted[j][i]);
            }
            key.clear();
            for(int i = 0; i < lenGuess; i++){
                if (keyShift[i] < 0) key.push_back(alphabet[keyShift[i]+26]);
                else key.push_back(alphabet[keyShift[i]]);
            }
        }
        }
        //Guess the key
        else{
            std::cout << "Please guess what the key is:\n";
            
            std::cin >> key;

            if (key.length() != lenGuess){
                std::cout << "That key is larger/smaller than the estimated key length\n";
            }
            else{

                //Simple decryption with assumed key, see part 1
                std::vector<int> valMess(encryption.length());
                std::vector<int> valKey(key.length());
                std::vector<int> finVal(encryption.length());

                for (int i = 0; i < encryption.length(); i++) {
                    for (int j = 0; j < alphabet.size(); j++){
                        if (encryption[i] == alphabet[j]) valMess[i] = j;
                    }
                }

                for (int i = 0; i < key.length(); i++) {
                    for (int j = 0; j < alphabet.size(); j++){
                        if (key[i] == alphabet[j]) valKey[i] = j;
                    }
                }

                for (int i = 0; i < encryption.length(); i++){
                    finVal[i] = (valMess[i] - valKey[i % valKey.size()]);
                    if (finVal[i] < 0) finVal[i] += 26;
                    finMessage.push_back(alphabet[finVal[i]]);
                }
            }
        }   
    }  
}