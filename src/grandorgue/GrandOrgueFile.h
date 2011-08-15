/*
 * GrandOrgue - free pipe organ simulator based on MyOrgan
 *
 * MyOrgan 1.0.6 Codebase - Copyright 2006 Milan Digital Audio LLC
 * MyOrgan is a Trademark of Milan Digital Audio LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#ifndef GRANDORGUEFILE_H
#define GRANDORGUEFILE_H

#include <vector>
#include "ptrvector.h"
#include <wx/wx.h>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include "GOrgueManual.h"

class GOrgueDisplayMetrics;
class GOrgueDivisionalCoupler;
class GOrgueEnclosure;
class GOrgueFrameGeneral;
class GOrgueGeneral;
class GOrgueLabel;
class GOrguePipe;
class GOrguePiston;
class GOrguePushbutton;
class GOrgueTremulant;
class GOrgueWindchest;

class GrandOrgueFile 
{

private:

	wxFileName m_path;
	int m_b_squash;
	GOrguePipe* m_compress_p;
	wxString m_filename;
	long m_elapsed;

	bool m_b_customized : 1;
	bool m_DivisionalsStoreIntermanualCouplers : 1;
	bool m_DivisionalsStoreIntramanualCouplers : 1;
	bool m_DivisionalsStoreTremulants : 1;
	bool m_GeneralsStoreDivisionalCouplers : 1;
	bool m_CombinationsStoreNonDisplayedDrawstops : 1;

	wxInt16 m_HighestSampleFormat;
	wxInt16 m_FirstManual;

	wxInt16 m_AmplitudeLevel;

	wxString m_HauptwerkOrganFileFormatVersion;
	wxString m_ChurchName;
	wxString m_ChurchAddress;
	wxString m_OrganBuilder;
	wxString m_OrganBuildDate;
	wxString m_OrganComments;
	wxString m_RecordingDetails;
	wxString m_InfoFilename;

	GOrgueDisplayMetrics* m_DisplayMetrics;

	ptr_vector<GOrgueEnclosure> m_enclosure;
	ptr_vector<GOrgueTremulant> m_tremulant;
	ptr_vector<GOrgueWindchest> m_windchest;
	ptr_vector<GOrguePiston> m_piston;
	ptr_vector<GOrgueLabel> m_label;
	ptr_vector<GOrgueGeneral> m_general;
	ptr_vector<GOrgueFrameGeneral> m_framegeneral;
	ptr_vector<GOrgueDivisionalCoupler> m_divisionalcoupler;
	ptr_vector<GOrgueManual> m_manual;

	std::vector<wxBitmap> m_images;

	void ReadOrganFile(wxFileConfig& odf_ini_file);
	bool TryLoad(wxInputStream* cache, wxProgressDialog& dlg, wxString& error);

public:

	GrandOrgueFile();
	wxString Load(const wxString& file, const wxString& file2 = wxEmptyString);
	void Save(const wxString& file);
	void Revert(wxFileConfig& cfg);
	void Abort();
	void PreparePlayback();
	~GrandOrgueFile(void);

	/* Access to the display metrics presented by the ODF */
	GOrgueDisplayMetrics* GetDisplayMetrics();

	void GenerateCacheHash(unsigned char hash[20]);
	/* Access to internal ODF objects */
	int GetManualAndPedalCount();
	int GetFirstManualIndex();
	GOrgueManual* GetManual(unsigned index);
	int GetTremulantCount();
	GOrgueTremulant* GetTremulant(unsigned index);
	int GetDivisionalCouplerCount();
	GOrgueDivisionalCoupler* GetDivisionalCoupler(unsigned index);
	int GetGeneralCount();
	GOrgueGeneral* GetGeneral(unsigned index);
	int GetNumberOfReversiblePistons();
	GOrguePiston* GetPiston(unsigned index);
	GOrgueFrameGeneral* GetFrameGeneral(unsigned index);
	GOrgueWindchest* GetWindchest(unsigned index);
	int GetWinchestGroupCount();
	GOrgueEnclosure* GetEnclosure(unsigned index);
	unsigned GetEnclosureCount();
	GOrgueLabel* GetLabel(unsigned index);
	int GetLabelCount();

	/* ODF general properties */
	bool DivisionalsStoreIntermanualCouplers();
	bool DivisionalsStoreIntramanualCouplers();
	bool DivisionalsStoreTremulants();
	bool CombinationsStoreNonDisplayedDrawstops();
	bool GeneralsStoreDivisionalCouplers();

	wxBitmap* GetImage(unsigned index);

	long GetElapsedTime();
	void SetElapsedTime(long elapsed);

	/* Overal amplitude of the organ when played back (used to prevent
	 * clipping, etc) */
	int GetAmplitude();

	/* TODO: can somebody figure out what this thing is */
	bool IsCustomized();

	/* Filename of the organ definition used to load */
	const wxString& GetODFFilename();

	/* Organ and Building general information */
	const wxString& GetChurchName();
	const wxString& GetChurchAddress();
	const wxString& GetOrganBuilder();
	const wxString& GetOrganBuildDate();
	const wxString& GetOrganComments();
	const wxString& GetRecordingDetails();
	const wxString& GetInfoFilename();

};

#endif
