#include "au.h"
#include "pch.h"
#include "wav.h"

auto typeOfAudioFile(const char* pFileName) -> e_audioFile
{
    // Ouverture en mode binaire.
    std::ifstream streamFile(pFileName, std::ifstream::binary);

    // On vérifie que le fichier soit bien ouvert.
    if (!streamFile.is_open())
    {
        std::cout << "Nous n'avons pas pu ouvrir le fichier: %s\n"
                  << pFileName << std::endl;
        return FILE_ERROR;
    }

    // On lit les 4 premiers octets, ils décrivent le format utilisé pour
    // chaque fichiers.
    char chFormat[4];

    streamFile.read(chFormat, sizeof(chFormat));

    streamFile.close();

    // On compare le format lue avec les différents formats et on retourne
    // le format supporté. Sinon on envoie une erreur.
    if (memcmp(chFormat, extAU, sizeof(chFormat)))
    {
        return FILE_AU;
    }
    else if (memcmp(chFormat, extRIFF, sizeof(chFormat)))
    {
        return FILE_RIFF;
    }
    else
    {
        return FILE_ERROR;
    }
}

int main(int numberOfArgs, char** pArgs)
{
    if (numberOfArgs < 2)
    {
        std::cout << "Pas assez d'arguments." << std::endl;
        return 0;
    }

    // Chaque arguments correspond à un fichier.
    for (int indexOfAudioFile = 1; indexOfAudioFile != numberOfArgs;
         indexOfAudioFile++)
    {
        auto pFilePath = pArgs[indexOfAudioFile];

        auto tOfAF = typeOfAudioFile(pFilePath);

        if (tOfAF == FILE_RIFF)
        {
            // Parser le fichier WAV.
            CWAVFile wavFile(pFilePath);

            if (!wavFile.IsValid())
            {
                std::cout
                  << "Le format n'est pas de type WAVE: " << pFilePath
                  << std::endl;
                continue;
            }

            // Dumper le fichier WAV.
            wavFile.Dump();

            // Jouer le fichier WAV.
            wavFile.Play();
        }
        else if (tOfAF == FILE_AU)
        {
            // Parser le fichier AU.
            CAUFile auFile(pFilePath);

            if (!auFile.IsValid())
            {
                std::cout
                  << "Le format n'est pas de type AU: " << pFilePath
                  << std::endl;
                continue;
            }

            // Dumper le fichier AU.
            auFile.Dump();

            // Jouer le fichier AU.
            auFile.Play();
        }
        else
        {
            std::cout << "Format non supporté dans le répertoire: "
                      << pFilePath << std::endl;
        }
    }

    return 0;
}
