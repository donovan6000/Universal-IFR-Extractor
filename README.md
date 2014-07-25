Universal IFR Extractor
=======================

Utility that can extract the internal forms representation from both EFI and UEFI modules and convert it into a human readable format.

You can download an executable <a href="http://bios-mods.com/pub/donovan6000/Software/Universal%20IFR%20Extractor/Universal%20IFR%20Extractor.exe">here.</a>

© 2014 donovan6000
<br /><br /><br />
Changelog:

v0.6.1 7/25/2014
* Print out the full fields of the Variable Store declaration, ie GUID and Name. Ex:
* 0x3B996 	Variable Store: 0xF015 {24 20 67 DE D0 3D D7 02 29 41 91 4A 9F 37 7C C3 4B 0D 15 F0 90 00 49 44 45 53 65 63 44 65 76 00}
* becomes
* 0x3B996 	Var Store: [0xF015][3DD0DE67-02D7-4129-914A-9F377CC34B0D][144][IDESecDev] {24 20 67 DE D0 3D D7 02 29 41 91 4A 9F 37 7C C3 4B 0D 15 F0 90 00 49 44 45 53 65 63 44 65 76 00}
* This allows trivial correlation in nvram with dmpstore (via EFI Shell).

v0.6 6/16/2014
* Added support for multiple string packages being used by one form.

v0.5 3/2/2014
* Fixed an issue that caused it to crash older versions of Windows.
* Corrected form set offsets that were off by four bytes.

v0.4 2/10/2014
* Fixed an issue with ASROCK FM2-A55M's Setup module. 

v0.3 2/8/2014
* Shows entire command's hex sequence in output.

v0.2 2/7/2014
* I think I fixed the issue that prevented it from running on Windows XP.
* Doesn't require any additional DLLs anymore.
* The extract dialog now starts in the selected modules directory.
* Fix the issue where it crashed on a few Acer BIOS. Thanks BDMaster!

v0.1 1/7/2014
* Initial release.
