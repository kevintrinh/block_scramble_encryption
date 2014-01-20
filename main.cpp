#include <iostream>
#include <string>
#include <cstring>
#include <fstream>


using namespace std;

struct block8bytes
{
    char _b[8];
};
struct block64bytes
{
    block8bytes _block[8];
};
struct block512bytes
{
    block64bytes _block64[8];
};


bool encrypt_file(char* _data, std::string _password, unsigned int _length, std::string _filename)
{
    /*Blocks lengths*/
    unsigned int blocks512 = 0;
    unsigned int blocks64= 0;
    unsigned int blocks8= 0;
    /*Current pos*/
    unsigned int data_pos = 0;

    /*Take length*/
    unsigned int length = _length;

    /*Take password*/
    int password_pattern[32];
    std::string password = _password;
    /*Take file buffer*/
    char file_buffer[_length];
    memcpy(file_buffer, _data, _length);

    /*Find password pattern*/
    for(int i=0; i<password.length(); i++)
    {
        if((((unsigned int)password[i]) % 2) == 0)
        {
            password_pattern[i] = 1;
        }
        else
        {
            password_pattern[i] =  0;
        }
    }
    for(int i=password.length(); i<32; i++)
    {
        /*Fill the rest of pattern with 0s*/
        password_pattern[i] = 0;
    }

    /*Take the size of the pattern buffer*/
    unsigned int password_pattern_size = sizeof(password_pattern) / sizeof(unsigned int);
    blocks512 = length/512;
    blocks64 = length / 64;
    blocks8 = length / 8;
    if((length % 512) > 0) blocks512++;
    if((length % 64) > 0) blocks64++;
    if((length % 8) > 0) blocks8++;


    block64bytes huge_blocks[blocks64];
    data_pos = 0;
    for(int i=0; i<sizeof(huge_blocks)/sizeof(block64bytes); i++)
    {
        for(int b=0; b<8; b++)
        {
            for(int c=0; c<8; c++)
            {
                if(data_pos <= length)
                {
                    char tmp_buff = file_buffer[data_pos];
                    for(int i=0; i<password.length(); i++)
                    {
                        tmp_buff ^= password[i];
                    }
                    huge_blocks[i]._block[b]._b[c] = tmp_buff;
                    data_pos++;
                }
            }
        }
    }
    /*Reorganize 8 blocks by password pattern*/


    for(int i=0; i<sizeof(huge_blocks)/sizeof(block64bytes); i++)
    {

        for(int pass = 0; pass<32; pass++)
        {

            if(password_pattern[pass] == 1)
            {
                block64bytes temp_64block;
                for(int take=0; take<8; take++)
                {
                    temp_64block._block[take] = huge_blocks[i]._block[take];
                }

                huge_blocks[i]._block[0] = temp_64block._block[1]; /*Takes one up*/
                huge_blocks[i]._block[1] = temp_64block._block[2];
                huge_blocks[i]._block[2] = temp_64block._block[3];
                huge_blocks[i]._block[3] = temp_64block._block[4];
                huge_blocks[i]._block[4] = temp_64block._block[5];
                huge_blocks[i]._block[5] = temp_64block._block[6];
                huge_blocks[i]._block[6] = temp_64block._block[7];
                huge_blocks[i]._block[7] = temp_64block._block[0];

            }
            if(password_pattern[pass] == 0)
            {
                block64bytes temp2_64block;
                for(int take=0; take<8; take++)
                {
                    temp2_64block._block[take] = huge_blocks[i]._block[take];
                }

                huge_blocks[i]._block[0] = temp2_64block._block[2]; /*Takes 2 up*/
                huge_blocks[i]._block[1] = temp2_64block._block[4];
                huge_blocks[i]._block[2] = temp2_64block._block[6];
                huge_blocks[i]._block[3] = temp2_64block._block[0];
                huge_blocks[i]._block[4] = temp2_64block._block[1];
                huge_blocks[i]._block[5] = temp2_64block._block[3];
                huge_blocks[i]._block[6] = temp2_64block._block[5];
                huge_blocks[i]._block[7] = temp2_64block._block[7];

            }
        }
    }

    /*Now reorganize 64 blocks*/
    /*Lets copy*/
    block512bytes huger_blocks[blocks512];
    unsigned int huger_blocks_pos = 0;
    unsigned int total_count = 0;

    for(int i=0; i<sizeof(huge_blocks)/sizeof(block64bytes); i++)
    {
        for(int b=0; b<8; b++)
        {
            if(total_count <= blocks64)
            {
                huger_blocks[huger_blocks_pos]._block64[b] = huge_blocks[i];
            }

            total_count++;
        }
        huger_blocks_pos++;
    }

    for(int i=0; i<sizeof(huger_blocks)/ sizeof(block512bytes); i++)
    {
        for(int pass=0; pass<32; pass++)
        {
            if(password_pattern[pass] == 1)
            {
                block512bytes temp_512block;
                for(int take=0; take<8; take++)
                {
                    temp_512block._block64[take] = huger_blocks[i]._block64[take];
                }
                huger_blocks[i]._block64[0] = temp_512block._block64[1]; /*Takes one up*/
                huger_blocks[i]._block64[1] = temp_512block._block64[2];
                huger_blocks[i]._block64[2] = temp_512block._block64[3];
                huger_blocks[i]._block64[3] = temp_512block._block64[4];
                huger_blocks[i]._block64[4] = temp_512block._block64[5];
                huger_blocks[i]._block64[5] = temp_512block._block64[6];
                huger_blocks[i]._block64[6] = temp_512block._block64[7];
                huger_blocks[i]._block64[7] = temp_512block._block64[0];
            }

            if(password_pattern[pass] == 0)
            {
                block512bytes temp_512block;
                for(int take=0; take<8; take++)
                {
                    temp_512block._block64[take] = huger_blocks[i]._block64[take];
                }
                huger_blocks[i]._block64[0] = temp_512block._block64[2]; /*Takes one up*/
                huger_blocks[i]._block64[1] = temp_512block._block64[4];
                huger_blocks[i]._block64[2] = temp_512block._block64[6];
                huger_blocks[i]._block64[3] = temp_512block._block64[0];
                huger_blocks[i]._block64[4] = temp_512block._block64[1];
                huger_blocks[i]._block64[5] = temp_512block._block64[3];
                huger_blocks[i]._block64[6] = temp_512block._block64[5];
                huger_blocks[i]._block64[7] = temp_512block._block64[7];
            }
        }
    }




    char encrypted_buffer[length];
    unsigned int out_pos = 0;
    for(int d=0; d<sizeof(huger_blocks)/sizeof(block512bytes); d++)
    {
        for(int i=0; i<8; i++)
        {
            for(int b=0; b<8; b++)
            {
                for(int c=0; c<8; c++)
                {
                    if(out_pos <= length)
                    {
                        encrypted_buffer[out_pos] = huger_blocks[d]._block64[i]._block[b]._b[c];
                        out_pos++;

                    }
                }
            }
        }


    }
    std::string new_name = _filename + ".encrypted";
    std::ofstream out_file(new_name.c_str(), ios::binary | ios::out);
    if(out_file.is_open())
    {
        if(out_file.good())
        {
            out_file.write(encrypted_buffer, length);
        }
    }
    std::cout << "File encrypted." << std::endl;
    out_file.close();


    return true;
}


bool decrypt_file(char* _data, std::string _password, unsigned int _length, std::string _filename)
{
    return true;
}


int main()
{
    /*Take filename*/
    std::string strFilename;
    std::fstream fOpen;
    do
    {
        fOpen.close();
        std::cout << "Enter the name of the file:\n";
        std::cin >> strFilename;
        fOpen.open(strFilename.c_str(), ios::in | ios::binary);
    }
    while(!fOpen.is_open());
    std::cout << "File " << strFilename<< " was successfully loaded." << std::endl;

    /*Take input if they want to encrypt or decrypt*/
    std::string strEncryptOrDecrypt;
    bool bToEncrypt;
    do
    {
        std::cout << "Do you want to encrypt or decrypt [e/d]?\n";
        std::cin >> strEncryptOrDecrypt;
        if(strEncryptOrDecrypt.compare("e") == 0)
        {
            bToEncrypt = true;
        }
        else if(strEncryptOrDecrypt.compare("e") == 0)
        {
            bToEncrypt = false;
        }
        else
        {
            std::cout << "Please enter a valid command" << std::endl;
        }
    }
    while(strEncryptOrDecrypt != "e" && strEncryptOrDecrypt != "d");

    std::string strPassword;
    do
    {
        std::cout << "Please enter a 128-bit password of 32 character lengths:\n";
        std::cin >> strPassword;
    }
    while(strPassword.length() == 0 || strPassword.length() > 32);

    unsigned int uiLength;
    fOpen.seekg(0,ios::end);
    uiLength = fOpen.tellg();
    fOpen.seekg(0, ios::beg);

    char buffer[uiLength];
    fOpen.read(buffer, uiLength);

    /*Verify with them that they are encrypting a certain file*/
    if(bToEncrypt)
    {
        encrypt_file(&buffer[0], strPassword, uiLength, strFilename);
    }
    else
    {
        decrypt_file(&buffer[0], strPassword, uiLength, strFilename);
    }

    fOpen.close();


    return 0;
}
