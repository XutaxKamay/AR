#include "wav.h"
#include "pch.h"

char extRIFF[] = { 'R', 'I', 'F', 'F' };

extern auto typeOfAudioFile(const char* pFileName) -> e_audioFile;

CWAVFile::CWAVFile(std::string sFileName)
 : m_sFileName(""), m_pFile(nullptr), m_Size(0), m_pHeader(nullptr)
{
    m_sFileName = sFileName;

    std::cout << "Ouverture du fichier: " << m_sFileName << std::endl;

    if (typeOfAudioFile(m_sFileName.c_str()) != FILE_RIFF)
    {
        std::cout << "Ce fichier n'est pas un fichier audio de format "
                     "RIFF (WAV): %s\n"
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

    m_pHeader = reinterpret_cast<psWAVHeader>(m_pFile);
}

// Déconstructeur.
CWAVFile::~CWAVFile()
{
    m_sFileName.clear();
    Free(m_pFile);
    m_pHeader = nullptr;
    m_Size    = 0;
}

// Taille en octets de toutes les données PCM.
auto CWAVFile::SizeOfData() -> size_t
{
    return m_pHeader->sizeOfData;
}

void CWAVFile::Dump()
{
    std::cout << "Taille du fichier audio (octets): " << m_Size
              << std::endl;
    std::cout << "Taille de l'en-tête (octets): " << sizeOfWAVHeader
              << std::endl;
    std::cout << "Type du format: " << GetTypeOfFormat() << std::endl;
    std::cout << "Nombre de canaux: " << m_pHeader->numberOfChannels
              << std::endl;
    std::cout << "Fréquence d'échantillonnages: " << m_pHeader->nFreq
              << " Hz" << std::endl;
    std::cout << "Octets par seconde à lire (o/s): "
              << m_pHeader->rateOfBytesSec << std::endl;
    std::cout << "Bloc d'échantillons pour tout les canaux (octets): "
              << m_pHeader->sizeOfBlock << std::endl;
    std::cout << "Nombre de bits par échantillonnage: "
              << m_pHeader->numberOfBitsPerSample << std::endl;
    std::cout << "Taille des données audio (octets): "
              << m_pHeader->sizeOfData << std::endl;
}

// 16 ou 8 bits par échantillon pour le moment supporté. Les fichiers wav
// sont en Little Endian par défaut.
auto CWAVFile::GetPulseAudioBitsSample() -> pa_sample_format
{
    if (m_pHeader->numberOfBitsPerSample == 16)
        return PA_SAMPLE_S16LE;

    return PA_SAMPLE_U8;
}

auto CWAVFile::Play() -> void
{
    int iErr;

    // Seulement PCM supporté pour le moment.
    if (m_pHeader->typeOfFormat == 1)
    {
        // Création des caréstéristiques d'un échantillon pour pulse
        // audio.
        pa_sample_spec sampleSpec = {
            .format   = GetPulseAudioBitsSample(),
            .rate     = m_pHeader->nFreq,
            .channels = (uint8_t)m_pHeader->numberOfChannels
        };

        // Création d'une instance/connection dans le serveur pulseaudio
        // pour pouvoir jouer le fichier audio.
        auto paConnection = pa_simple_new(0,
                                          "Playing sample wav file",
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
}

// Seulement PCM supporté.
auto CWAVFile::GetTypeOfFormat() -> const char*
{
    if (m_pHeader->typeOfFormat == 1)
        return "PCM";

    return "ERROR";
}