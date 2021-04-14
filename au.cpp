#include "au.h"
#include "pch.h"

char extAU[] = { '.', 's', 'n', 'd' };

extern auto typeOfAudioFile(const char* pFileName) -> e_audioFile;

CAUFile::CAUFile(std::string sFileName)
 : m_sFileName(""), m_pFile(nullptr), m_Size(0), m_pHeader(nullptr)
{
    m_sFileName = sFileName;

    std::cout << "Ouverture du fichier: " << m_sFileName << std::endl;

    if (typeOfAudioFile(m_sFileName.c_str()) != FILE_AU)
    {
        std::cout << "Ce fichier n'est pas un fichier audio de format "
                     "AU: %s\n"
                  << m_sFileName << std::endl;
        return;
    }

    // On ouvre le fichier en mode binaire.
    std::ifstream streamFile(m_sFileName, std::ifstream::binary);

    // On vérifie que le fichier soit bien ouvert.
    if (!streamFile.is_open())
    {
        std::cout << "Nous n'avons pas pu ouvrir le fichier: %s\n"
                  << m_sFileName << std::endl;
        return;
    }

    // Mettre la position du curseur à la fin du fichier.
    streamFile.seekg(0, streamFile.end);

    // Obtenir la position du curseur pour connaître la taille en mode
    // binaire.
    m_Size = streamFile.tellg();

    // Remettre le curseur à la position de départ.
    streamFile.seekg(0, streamFile.beg);

    // On alloue de la mémoire
    m_pFile = Alloc(m_Size);

    // On copie tout le fichier en mode binaire à l'adresse m_pFile.
    streamFile.read(reinterpret_cast<char*>(m_pFile), m_Size);

    // On ferme le fichier.
    streamFile.close();

    m_pHeader = reinterpret_cast<psAUHeader>(m_pFile);

    // Swaper les bytes en Little Endian car le format AU est encodé avec
    // du Big Endian.
    m_pHeader->offsetOfData     = swapEndian(m_pHeader->offsetOfData);
    m_pHeader->sizeOfData       = swapEndian(m_pHeader->sizeOfData);
    m_pHeader->typeOfFormat     = swapEndian(m_pHeader->typeOfFormat);
    m_pHeader->nFreq            = swapEndian(m_pHeader->nFreq);
    m_pHeader->numberOfChannels = swapEndian(m_pHeader->numberOfChannels);
}

CAUFile::~CAUFile()
{
    m_sFileName.clear();
    Free(m_pFile);
    m_pHeader = nullptr;
    m_Size    = 0;
}

auto CAUFile::SizeOfData() -> size_t
{
    return m_pHeader->sizeOfData;
}

auto CAUFile::Dump() -> void
{
    std::cout << "Taille du fichier audio (octets): " << m_Size
              << std::endl;
    std::cout << "Taille de l'en-tête (octets): " << sizeOfAUHeader
              << std::endl;
    std::cout << "Type du format: " << GetTypeOfFormat() << std::endl;
    std::cout << "Nombre de canaux: " << (m_pHeader->numberOfChannels)
              << std::endl;
    std::cout << "Fréquence d'échantillonnages: " << (m_pHeader->nFreq)
              << " Hz" << std::endl;
    std::cout << "Taille des données audio (octets): "
              << (m_pHeader->sizeOfData) << std::endl;
    std::cout << "Poistion des données audio (octets): "
              << (m_pHeader->offsetOfData) << std::endl;
}

/*
1 = 8-bit G.711 µ-law
2 = 8-bit linear PCM
3 = 16-bit linear PCM
4 = 24-bit linear PCM
5 = 32-bit linear PCM
6 = 32-bit à virgule flottante (standard IEEE_754)
7 = 64-bit à virgule flottante (standard IEEE_754)
8 = Données d'échantillons fragmentés
9 = Programme DSP
10 = 8-bit à virgule fixe
11 = 16-bit à virgule fixe
12 = 24-bit à virgule fixe
13 = 32-bit à virgule fixe
18 = 16-bit linéaire "with emphasis"
19 = 16-bit linéaire compressé
20 = 16-bit linéaire "with emphasis" et compressé
21 = Commandes DSP pour Music kit
23 = 4-bit ISDN u-law compressé en utilisant la norme de compression ITU-T
G.721 ADPCM dédié à la voix. 27 = 8-bit G.711 A-law

 typedef enum pa_sample_format {
 PA_SAMPLE_U8,
 PA_SAMPLE_ALAW,
 PA_SAMPLE_ULAW,
 PA_SAMPLE_S16LE,
 PA_SAMPLE_S16BE,
 PA_SAMPLE_FLOAT32LE,
 PA_SAMPLE_FLOAT32BE,
 PA_SAMPLE_S32LE,
 PA_SAMPLE_S32BE,
 PA_SAMPLE_S24LE,
 PA_SAMPLE_S24BE,
 PA_SAMPLE_S24_32LE,
 PA_SAMPLE_S24_32BE,
 PA_SAMPLE_MAX,
 PA_SAMPLE_INVALID = -1
} pa_sample_format_t;
*/

auto CAUFile::GetPulseAudioBitsSample() -> pa_sample_format
{
    if (m_pHeader->typeOfFormat == 1)
    {
        return PA_SAMPLE_ULAW;
    }
    else if (m_pHeader->typeOfFormat == 2)
    {
        return PA_SAMPLE_U8;
    }
    else if (m_pHeader->typeOfFormat == 3)
    {
        return PA_SAMPLE_S16BE;
    }
    else if (m_pHeader->typeOfFormat == 4)
    {
        return PA_SAMPLE_S24BE;
    }
    else if (m_pHeader->typeOfFormat == 5)
    {
        return PA_SAMPLE_S32BE;
    }
    else if (m_pHeader->typeOfFormat == 6)
    {
        return PA_SAMPLE_FLOAT32BE;
    }
    else if (m_pHeader->typeOfFormat == 27)
    {
        return PA_SAMPLE_ALAW;
    }
    else
    {
        return PA_SAMPLE_INVALID;
    }
}

auto CAUFile::GetTypeOfFormat() -> const char*
{
    if (m_pHeader->typeOfFormat == 1)
    {
        return "8-bit G.711 µ-law";
    }
    else if (m_pHeader->typeOfFormat == 2)
    {
        return "8-bit linéaire PCM";
    }
    else if (m_pHeader->typeOfFormat == 3)
    {
        return "16-bit linéaire PCM";
    }
    else if (m_pHeader->typeOfFormat == 4)
    {
        return "24-bit linéaire PCM";
    }
    else if (m_pHeader->typeOfFormat == 5)
    {
        return "32-bit linéaire PCM";
    }
    else if (m_pHeader->typeOfFormat == 6)
    {
        return "32-bit à virgule flottante (standard IEEE_754) Big "
               "Endian.";
    }
    else if (m_pHeader->typeOfFormat == 27)
    {
        return "8-bit G.711 A-law";
    }
    else
    {
        return "ERROR";
    }
}

auto CAUFile::Play() -> void
{
    int iErr;

    // Création des caréstéristiques d'un échantillon pour pulse audio.
    pa_sample_spec sampleSpec = {
        .format   = GetPulseAudioBitsSample(),
        .rate     = m_pHeader->nFreq,
        .channels = (uint8_t)m_pHeader->numberOfChannels
    };

    // Création d'une instance/connection dans le serveur pulseaudio pour
    // pouvoir jouer le fichier audio.
    auto paConnection = pa_simple_new(0,
                                      "Playing sample snd file",
                                      PA_STREAM_PLAYBACK,
                                      0,
                                      "Music",
                                      &sampleSpec,
                                      0,
                                      0,
                                      &iErr);

    // Ecrit les données PCM dans le serveur.
    pa_simple_write(paConnection, GetData(), SizeOfData(), &iErr);

    std::cout << "write: " << iErr << std::endl;

    // Attendre que le fichier audio se joue.
    pa_simple_drain(paConnection, &iErr);

    std::cout << "play: " << iErr << std::endl;

    // Finit, on se déconnecte du serveur pulseaudio.
    pa_simple_free(paConnection);
}