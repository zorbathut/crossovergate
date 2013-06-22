/********************************************************************************
 Copyright (C) 2013 Lucas Murray <lmurray@undefinedfire.com>
 Copyright (C) 2013 Ben Wilhelm <zorba-crossovergate@pavlovian.net.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
********************************************************************************/

#pragma once

#include "OBSApi.h"

class CrossoverGate;

#define CONFIG_FILENAME TEXT("\\CrossoverGate.ini")

//============================================================================
// CrossoverGateFilter class

class CrossoverGateFilter : public AudioFilter
{
    //-----------------------------------------------------------------------
    // Private members

private:
    CrossoverGate *     parent;
    
    // State
    float   attenuation; // Current gate multiplier
    float   level;  // Input level with delayed decay
    float   heldTime; // The amount of time we've held the gate open after it we hit the close threshold
    bool    isOpen;

    //-----------------------------------------------------------------------
    // Constructor/destructor
    
public:
    CrossoverGateFilter(CrossoverGate *parent);
    virtual ~CrossoverGateFilter();

	bool IsOpen() const { return isOpen; }
    
    //-----------------------------------------------------------------------
    // Methods

public:
    virtual AudioSegment *Process(AudioSegment *segment);

private:
    void ApplyCrossoverGate(float *buffer, int totalFloats);
};

//============================================================================
// CrossoverGateFilter class

class DesktopGateFilter : public AudioFilter
{
    //-----------------------------------------------------------------------
    // Private members

private:
    CrossoverGate *     parent;
	CrossoverGateFilter	* ngf;
    
    // State
    float   attenuation; // Current gate multiplier
    float   level;  // Input level with delayed decay
    float   heldTime; // The amount of time we've held the gate open after it we hit the close threshold
    bool    isOpen;

    //-----------------------------------------------------------------------
    // Constructor/destructor
    
public:
    DesktopGateFilter(CrossoverGate *parent, CrossoverGateFilter *ngf);
    virtual ~DesktopGateFilter();
    
    //-----------------------------------------------------------------------
    // Methods

public:
    virtual AudioSegment *Process(AudioSegment *segment);

private:
    void ApplyCrossoverGate(float *buffer, int totalFloats);
};

//============================================================================
// CrossoverGateSettings class

class CrossoverGateSettings : public SettingsPane
{
    //-----------------------------------------------------------------------
    // Constants

private:
    static const int    REPAINT_TIMER_ID = 1;
    static const int    CURVOL_RESOLUTION = 96 * 4 - 1; // 0.25 dB resolution for a (-96..0) dB range

    //-----------------------------------------------------------------------
    // Private members

private:
    CrossoverGate * parent;

    //-----------------------------------------------------------------------
    // Constructor/destructor

public:
    CrossoverGateSettings(CrossoverGate *parent);
    virtual ~CrossoverGateSettings();

    //-----------------------------------------------------------------------
    // Methods

private:
    void SetTrackbarCaption(int controlId, int db);
    void RepaintVolume();
    void RefreshConfig();

    // Message processing
    void MsgInitDialog();
    void MsgDestroy();
    INT_PTR MsgClicked(int controlId, int code, HWND controlHwnd);
    INT_PTR MsgScroll(bool vertical, WPARAM wParam, LPARAM lParam);
    INT_PTR MsgTimer(int timerId);

public:
    // Interface
    virtual CTSTR GetCategory() const;
    virtual HWND CreatePane(HWND parentHwnd);
    virtual void DestroyPane();
    virtual INT_PTR ProcMessage(UINT message, WPARAM wParam, LPARAM lParam);
    virtual void ApplySettings();
    virtual void CancelSettings();
    virtual bool HasDefaults() const;
    virtual void SetDefaults();
};

//============================================================================
// CrossoverGate class

class CrossoverGate
{
    friend class CrossoverGateFilter;
    friend class CrossoverGateSettings;

    //-----------------------------------------------------------------------
    // Static members

public:
    static HINSTANCE    hinstDLL;
    static CrossoverGate *  instance;
    
    //-----------------------------------------------------------------------
    // Private members

private:
    AudioSource *       micSource;
    CrossoverGateFilter *   filter;
    CrossoverGateSettings * settings;
    ConfigFile          config;
    bool                isDisabledFromConfig;

	AudioSource *       desktopSource;
	DesktopGateFilter * desktopFilter;

    // User configuration
    bool    isEnabled;
    float   openThreshold;
    float   closeThreshold;
    float   attackTime;
    float   holdTime;
    float   releaseTime;

    //-----------------------------------------------------------------------
    // Constructor/destructor
    
public:
    CrossoverGate();
    ~CrossoverGate();

    //-----------------------------------------------------------------------
    // Methods

public:
    void LoadDefaults();
    void LoadSettings();
    void SaveSettings();
    void StreamStarted();
    void StreamStopped();
};

//============================================================================
// Plugin entry points

extern "C" __declspec(dllexport) bool LoadPlugin();
extern "C" __declspec(dllexport) void UnloadPlugin();
extern "C" __declspec(dllexport) void OnStartStream();
extern "C" __declspec(dllexport) void OnStopStream();
extern "C" __declspec(dllexport) CTSTR GetPluginName();
extern "C" __declspec(dllexport) CTSTR GetPluginDescription();
