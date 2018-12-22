
#ifndef DYNAMIC_VARIABLE_H
#define DYNAMIC_VARIABLE_H
#ifdef _MSC_VER
#pragma once
#endif


//----------------------------------------------------------------------------
// DynamicVar
//----------------------------------------------------------------------------


class DynamicVar 
{
public:
    DynamicVar()
    {
        m_fVar = 0;
        m_fTime = 0;
        m_fTimeTarget = 0;
    }

    void setVar(float fVal, float fTime)
    {
		m_fBase = m_fVar;
        m_fTime = 0.0f;
        m_fTarget = fVal;
        m_fDelta = m_fTarget - m_fVar;
        m_fTimeTarget = m_fTime + fTime;
    }
    void update(float fDeltaTime)
    {
        m_fTime += fDeltaTime;
        m_fVar = m_fBase + (m_fTime / m_fTimeTarget) * m_fDelta;
        if (hasReachedTarget())
        {
            m_fVar = m_fTarget;
        }
    }

    operator float()
    {  return m_fVar;  }

    void operator = (float fVal)
    {  
        m_fVar = fVal;
        m_fTarget = fVal;
        m_fTime = 0.0f;
        m_fTimeTarget = 0.0f;
    }

    bool hasReachedTarget()
    {  return (m_fTime >= m_fTimeTarget);  }

private:
	float	m_fBase;
    float   m_fVar;
    float   m_fTarget;
    float   m_fDelta;
    float   m_fTime;
    float   m_fTimeTarget;
};



// SoundEnvelope
struct SoundEnvelope
{
public:
    SoundEnvelope()
    {  clear();  }
    void clear()
    {
        m_fInitialTime = 0.0f; 
        m_fAttackTime = 0.0f;
        m_fAttackLevel = 0.0f; 
        m_fDecayTime = 0.0f;
        m_fDecayLevel = 0.0f;
        m_fSustainTime = 0.0f;
        m_fSustainLevel = 0.0f;
        m_fReleaseTime = 0.0f;
        m_fReleaseLevel = 0.0f;
    }

    float m_fInitialTime; 
    float m_fAttackTime;
    float m_fAttackLevel; 
    float m_fDecayTime;
    float m_fDecayLevel;
    float m_fSustainTime;
    float m_fSustainLevel;
    float m_fReleaseTime;
    float m_fReleaseLevel;
};


// EnvelopeVar
class EnvelopeVar
{
public:

    enum Progression
    {
        ENV_FLOOR,
        ENV_INITIAL,
        ENV_ATTACK,
        ENV_RELEASE,
        ENV_SUSTAIN,
        ENV_DECAY
    };
    EnvelopeVar()
    {  clear();  }
    void clear()
    {
        m_Envelope.clear();
        m_Level = 0.0f;
        m_eState = ENV_FLOOR;
    }

    void setVar(const SoundEnvelope& env)
    {
        m_Envelope = env;
        m_eState = ENV_INITIAL;
        m_Level.setVar(
            m_Level, m_Envelope.m_fInitialTime);
    }

    operator float()
    {  return m_Level;  }

    void update(float fDeltaTime)
    {
        m_Level.update(fDeltaTime);
        if(m_Level.hasReachedTarget())
        {
            switch(m_eState)
            {
            case ENV_FLOOR:
                break;
            case ENV_INITIAL:
                m_eState = ENV_ATTACK;
                m_Level.setVar(
                    m_Envelope.m_fAttackLevel, 
                    m_Envelope.m_fAttackTime);
                break;
            case ENV_ATTACK:
                m_eState = ENV_DECAY;
                m_Level.setVar(
                    m_Envelope.m_fDecayLevel, 
                    m_Envelope.m_fDecayTime);
                break;
            case ENV_DECAY:
                m_eState = ENV_SUSTAIN;
                m_Level.setVar(
                    m_Envelope.m_fSustainLevel,           
                    m_Envelope.m_fSustainTime);
                break;
            case ENV_SUSTAIN:
                m_eState = ENV_RELEASE;
                m_Level.setVar(
                    m_Envelope.m_fReleaseLevel, 
                    m_Envelope.m_fReleaseTime);
                break;
            case ENV_RELEASE:
                m_eState = ENV_FLOOR;
                break;
            };
        }    
	}

private:
    SoundEnvelope   m_Envelope;
    DynamicVar      m_Level;
    Progression     m_eState;
};
	



#endif // DYNAMIC_VARIABLE_H
