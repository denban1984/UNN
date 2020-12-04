#include "UploadTosite.h"


LoadSettings::LoadSettings():SettFile(fs::current_path().string()), Fpath(SettFile)
{
	std::string ErorLogFile{ fs::current_path().string() + "\\"+ currentDateTime(1)+"_ErrorLog.log" };
	std::string LogFile{ fs::current_path().string() + "\\" + currentDateTime(1) + "_Log.log" };
	/*ErrorLog.open(ErorLogFile);
	Log.open(LogFile);*/
	SaveLoadParam(SettFile, 1);
}

LoadSettings::LoadSettings(std::string str): SettFile(fs::current_path().string()), Fpath(str)
{
	std::string ErorLogFile{ fs::current_path().string() + "\\" + currentDateTime(1) + "_ErrorLog.log" };
	std::string LogFile{ fs::current_path().string() + "\\" + currentDateTime(1) + "_Log.log" };
	/*ErrorLog.open(ErorLogFile);
	Log.open(LogFile);*/
	SaveLoadParam(SettFile, 1);
}

LoadSettings::LoadSettings(std::string str, std::string fstr): SettFile(str), Fpath(fstr)
{
	std::string ErorLogFile{ fs::current_path().string() + "\\" + currentDateTime(1) + "_ErrorLog.log" };
	std::string LogFile{ fs::current_path().string() + "\\" + currentDateTime(1) + "_Log.log" };
	/*ErrorLog.open(ErorLogFile);
	Log.open(LogFile);*/
	SaveLoadParam(SettFile, 1);
}

LoadSettings::~LoadSettings()
{
	Log << currentDateTime(0) << "_" << "Everything done, I hope, correctly. Goodbye! \n";
	/*ErrorLog.flush();
	ErrorLog.close();
	Log.flush();
	Log.close();*/
}

std::string LoadSettings::EncDecryptStr(std::string Mstring)
{
	std::string tmp; int i;

	for (auto it = begin(Mstring); it != end(Mstring); ++it)
	{
		i = ((int)(*it)) ^ PrKey;
		tmp.push_back((char)(i));
	}

	return tmp;

}

void LoadSettings::SaveLoadParam(std::string s, int x)
{
	
	const fs::path IniFile = s + R"(\)" + setFileName;
	std::string str;
	std::size_t pos_delimiter;

	if (x > 0) { 			//загрузка данных перед запуском
		Log << currentDateTime(0) << "_" << "Load param from: " << IniFile <<'\n';
		if (fs::exists(IniFile)) {
			std::ifstream Filenastr(IniFile);
			while (!Filenastr.eof()) {
				Filenastr >> str;
				str = EncDecryptStr(str);
				pos_delimiter = str.find("|");
				if (pos_delimiter != std::string::npos) { NastrMap.insert(std::pair<std::string, std::string>(str.substr(0, pos_delimiter), str.substr(pos_delimiter + 1))); }
				str = "";
			}
			Log << currentDateTime(0) << "_" << "NastrMap Volume = "<< NastrMap.size() << '\n';

		}
		else {
			std::cout << IniFile << " - Can't find file!";
			ErrorLog<< currentDateTime(0) << "_" << currentDateTime(0)<<"_" << " - Can't find file!\n";
			
					}
	}
	else {
		//сохранение данных перед закрытием
		Log <<currentDateTime(0) << "_" << "Save param before quit in: " << IniFile << '\n';
		if (fs::exists(IniFile)) {
			try {
				fs::remove(IniFile);
			}
			catch (...) { std::cout << "Can't delete file!\n"; ErrorLog<<currentDateTime(0) << "_"  << IniFile << " - Can't delete file!\n"; 
			}
		}
		std::ofstream Out_Filenastr;
		Out_Filenastr.open(IniFile);
		for (auto it = NastrMap.begin(); it != NastrMap.end(); ++it)
		{
			Out_Filenastr << EncDecryptStr((*it).first + '|' + (*it).second) << '\n';
		}
		NastrMap.clear();
		Out_Filenastr.flush();
		Out_Filenastr.close();
	};



}

Parse::Parse() :LoadSettings()
{
	GetSubFolders(fs::current_path().string());
}

Parse::Parse(std::string T): LoadSettings(T)
{
	GetSubFolders(T);
}

Parse::Parse(std::string sT, std::string fsT): LoadSettings(sT,fsT)
{
	GetSubFolders(fsT);
}

Parse::~Parse()
{
	subdirs.clear();
}

void Parse::GetSubFolders(std::string fp)
{
	
	Log << currentDateTime(0) << "_" << "Get sub-folders from " << fp << '\n';
	fs::directory_iterator begin(fp);
	fs::directory_iterator end;
	std::copy_if(begin, end, std::back_inserter(subdirs), [](const fs::path & path) {
		return fs::is_directory(path);
		});
	if (subdirs.size() == 0) { subdirs.emplace_back(fp); }
	Log << currentDateTime(0) << "_" << "MAP subdirs volume " << subdirs.size() << '\n';
}

GetFilesByMask::GetFilesByMask():Parse()
{
	GetFilesListMap();
}

GetFilesByMask::GetFilesByMask(std::string T):Parse(T)
{
	GetFilesListMap();
}

GetFilesByMask::GetFilesByMask(std::string sT, std::string fsT):Parse(sT,fsT)
{
	GetFilesListMap();
}

GetFilesByMask::~GetFilesByMask()
{
	for (auto it = begin(MapFiles); it != end(MapFiles); ++it) {
		(it->second)->clear();
		delete (it->second);
	}
	MapFiles.clear();
}

void GetFilesByMask::setmask(std::string str)
{
	mask.push_back(str);
}

void GetFilesByMask::clear_mask()
{
	mask.clear();
}

void GetFilesByMask::GetFilesListMap()
{
	Log << currentDateTime(0) << "_" << "GetFilesListMap. mask= ";
	for (auto m_n : mask){ Log << m_n << ';'; }
	std::string fnam,er;
	for (auto it = begin(subdirs); it != end(subdirs); ++it) {

		fs::recursive_directory_iterator begin(*it);
		fs::recursive_directory_iterator end;
		std::vector<fs::path>* fileslist = new std::vector<fs::path>;
		
		for (fs::recursive_directory_iterator pit = begin; pit != end;++pit) {
		const fs::path& path = *pit;
			if (fs::is_regular_file(path)) {
				for (auto m : mask) { if (path.extension() == m) { 
					//проверка на корректность имени файла
					fnam = path.string();
					fnam=fnam.substr(fnam.rfind("\\")+1);
					for (auto a : fnam) {

						if ((static_cast<int>(a) >0 && static_cast<int>(a) < 45) || (static_cast<int>(a) <= 64 && static_cast<int>(a) >= 58) ||(static_cast<int>(a)<=94 && static_cast<int>(a)>=91)||
							(static_cast<int>(a)<=127 && static_cast<int>(a)>=123)) { 
								errorMsg NeR(fnam+"\n BAD FILE NAME! FORBIDDEN SYMBOL '" + a +"'\n FIX THE ERROR AND TRY AGAIN! \n Press any key!  ");
							throw(NeR);
						}
						else { ; }
					}
					fileslist->push_back(path);} }
			}
		
		}
		Log << '\n';
		MapFiles.insert(std::pair<fs::path, std::vector<fs::path>*>(*it, fileslist));		
	}
	Log << currentDateTime(0) << "_" << "MapFiles volume = " << MapFiles.size() << '\n';
}

CreateZip::CreateZip():GetFilesByMask()
{
	MakeZipFiles();
}

CreateZip::CreateZip(std::string T): GetFilesByMask(T)
{
	MakeZipFiles();
}

CreateZip::CreateZip(std::string sT, std::string fsT): GetFilesByMask(sT,fsT)
{
	MakeZipFiles();
}

CreateZip::~CreateZip()
{
	ZipFiles.clear();
}

void CreateZip::MakeZipFiles()
{
	std::string ArchName;
	std::string FoldName;
	std::string ZipStr;
	fs::path res;

	ZipFiles.reserve(100);
	std::string preChStr(env_var);
	for (auto it = begin(MapFiles); it != end(MapFiles); ++it) {
	    ArchName = "";
		FoldName = "";
		FoldName = (it->first).string();
		FoldName=FoldName.substr(FoldName.rfind("\\"));
		ArchName = (it->first).string()+FoldName + ArchName+ sufstr+ currentDateTime(0) + ".zip";
		Log << currentDateTime(0) << "_" << " Arch File: " << ArchName << '\n';
		if (fs::exists(ArchName)) {
			try {
				fs::remove(ArchName);
			}
			catch (...) {
				std::cout << "Can't delete file! - " << ArchName; ErrorLog << currentDateTime(0) << "_" << "Can't delete old file! - " << ArchName << '\n'; 
			}
		}
		ZipFiles.emplace_back(ArchName);
		for (auto iit = begin(*it->second); iit != end(*it->second); ++iit) {
				ZipStr = preChStr + ArchName + " " + iit->string();
				try {
					system(ZipStr.c_str());
					Log << currentDateTime(0) << "_"<<"File: "<< iit->string() << " Append to ZIP - "<<ArchName<<'\n';
				}
				catch (...) { ErrorLog << currentDateTime(0) << "_" << " ZIP create ERROR! - " << ZipStr << '\n';
				}
		   	}
		   }
}

const std::string LoadSettings::currentDateTime(int i)
{
	time_t now = time(NULL);
	struct tm  tstruct;
	char       buf[80];
	localtime_s(&tstruct,&now);
	if (i == 0) { strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tstruct); }
	if (i != 0) { strftime(buf, sizeof(buf), "%Y%m%d", &tstruct); }
	return buf;
}

void UploadFiles::UpLoad()
{
	CInternetSession sess(_T("Upload Doc Session"));
	CFtpConnection* pConnect = NULL;
	Log << currentDateTime(0) << "_" << "Upload Doc Session BEGIN" << '\n';


	auto nmiter = NastrMap.find("host");
	std::wstring wphost(((*nmiter).second).begin(), ((*nmiter).second).end());


	nmiter = NastrMap.find("user");
	std::wstring wpuser(((*nmiter).second).begin(), ((*nmiter).second).end());

	nmiter = NastrMap.find("pswd");
	std::wstring wppass(((*nmiter).second).begin(), ((*nmiter).second).end());

	nmiter = NastrMap.find("port");
	int lpport = std::stoi((*nmiter).second);

	nmiter = NastrMap.find("passmode");
	int lppassmode = std::stoi((*nmiter).second);
	
	nmiter = NastrMap.find("hostfold");
    std::string s = (*nmiter).second;
    std::string s_tmp;
    std::for_each(((*nmiter).second).begin(), ((*nmiter).second).end(), [&s_tmp](char n) {(n == '/') ? s_tmp += "//" : s_tmp += n; });
	std::wstring wphostfold(s_tmp.begin(), s_tmp.end());

	try {
		pConnect = sess.GetFtpConnection(wphost.c_str(),wpuser.c_str(), wppass.c_str(), lpport, lppassmode);
	}
		catch (...) {
			std::cout << "Connection ERROR \n";
			ErrorLog << currentDateTime(0) << "_" << "Connection ERROR \n";
			/*....*/
		}
		if (pConnect != NULL) {
			std::cout << "Connection established" << '\n'; Log << currentDateTime(0) << "_" << "Connection established" << '\n';
		}
		try {
			pConnect->SetCurrentDirectory(wphostfold.c_str());
			Log << currentDateTime(0) << "_" << "CH_DIR\n";
		}
	catch (...) {
		std::cout << "CH_DIR ERROR \n";
		ErrorLog << currentDateTime(0) << "_" << "CH_DIR ERROR\n"; 
		/*....*/}
	CString str;
	pConnect->GetCurrentDirectory(str);
	std::wstring ws (str);
	std::string sstr = std::string(ws.begin(), ws.end());
	std::cout << "Curent directory "<<sstr << '\n';
	Log << currentDateTime(0) << "_" << "Current directory " << sstr << '\n';
	std::wstring wpath;
	std::wstring wname;
	std::string sname;
	int i;
	for (auto iter = ZipFiles.begin(); iter != ZipFiles.end(); iter++) { 
		sname= (*iter).substr((*iter).rfind(R"(\)") + 1);
		for (auto wpath_iter = (*iter).begin(); wpath_iter != (*iter).end(); wpath_iter++) { wpath.push_back(*wpath_iter); }
		for (auto wname_iter = sname.begin(); wname_iter != sname.end(); wname_iter++) { wname.push_back(*wname_iter); }

		if (fs::exists((*iter))) {
			try {
				if (pConnect != NULL) { i=pConnect->PutFile(wpath.c_str(), wname.c_str()); 
				if (i > 0) {
					std::cout << (*iter) << " Upload sucsessful \n"; Log << currentDateTime(0) << "_" << (*iter) << " Upload sucsessful \n";
				}
				else {
				ErrorLog << currentDateTime(0) << "_" << (*iter) << " Upload ERROR! Last ERROR Code= " << GetLastError() << '\n'; 
					std::cout << (*iter) << " Upload ERROR! "<<GetLastError()<<'\n'; }
				}
				else { std::cout << "LOST Connection before put file \n";ErrorLog << currentDateTime(0) << "_" << "LOST Connection before put file \n";
				}
			}
			catch (...) { std::cout << "ERROR PUT FILE\n"; ErrorLog<< currentDateTime(0) << "_" <<  "ERROR PUT FILE\n";/*....*/ }
		}
		else { std::cout << " LOST FILE: " << (*iter) << " not found! \n";ErrorLog << currentDateTime(0) << "_" << " LOST FILE: " << (*iter) << " not found! \n";
		};

		sname.clear();
		wpath.clear();
		wname.clear();
		 }
	
	if (pConnect != NULL) {
		
		try {
			pConnect->Close();
			delete pConnect;
			Log << currentDateTime(0) << "_" << "Upload Doc Session CLOSE\n";
		}
		catch (...) {
			std::cout << "I want to terminate connection, but I can't and I don't know why but I know last error code = " << GetLastError() << '\n'; ErrorLog << currentDateTime(0) << "_" << "I want to terminate connection, but I can't and I don't know why but I know last error code = " << GetLastError() << '\n'; 
		}
	}
}

UploadFiles::UploadFiles():CreateZip()
{
	UpLoad();
}

UploadFiles::UploadFiles(std::string T): CreateZip(T)
{
	UpLoad();
}

UploadFiles::UploadFiles(std::string sT, std::string fsT): CreateZip(sT,fsT)
{
	UpLoad();
}

UploadFiles::~UploadFiles()
{
}

ParsingKeys::ParsingKeys(std::vector<std::string> keys)
{

	if (keys[0] == "-fp") {
		
		if (keys.size()>1 && keys[1]!="") { 
			try { UploadFiles BaseCl(keys[1]); }
			catch (errorMsg err) {
				std::cout << err.ErrMsg;
				Farewell_msg = "\n An error was detected, the program will be interrupted. \n Correct the error and run the program again.";
				std::cin.get();
			}
		}
		else { std::cout << "Not enough parameters for key -fp \n"; }
	}
	if (keys[0] == "-iffp") {
		if (keys.size() >= 3 || keys[1] != "" || keys[2] != "") {
			try { UploadFiles BaseCl(keys[1], keys[2]); }
			catch (errorMsg err) {
				std::cout << err.ErrMsg;
				Farewell_msg = "\n An error was detected, the program was interrupted. \n Correct the error and run the program again.";
				std::cin.get();
			}
		}
		else { std::cout << "Not enough parameters for key -iffp \n"; }
	}
	if (keys[0] == "-ini") { 
		if (keys.size() > 1 && keys[1] != "") { ManageIni Init(keys[1]); }
		else { ManageIni Init(fs::current_path().string()); }
		
	}
	std::cout << Farewell_msg;
}

ParsingKeys::~ParsingKeys()
{
}

ManageIni::ManageIni():LoadSettings()
{
	std::string pswd;
	std::cout << "\n Enter password: ";
	std::cin >> pswd;
	if (pswd == "175qwe%w") { ManageINI(NastrMap);}	else { std::cout << "Wrong! \n"; }

}

ManageIni::ManageIni(std::string str):LoadSettings(str,"")
{
	std::string pswd;
	std::cout << "\n Enter password: ";
	std::cin >> pswd;
	if (pswd == "175qwe%w") { ManageINI(NastrMap); } else { std::cout << "Wrong! \n"; }

}

ManageIni::~ManageIni()
{
	TmpNastrMap.clear();

}

void ManageIni::ManageINI(std::map<std::string, std::string>& nmap) {
	std::string d,j,nv;
	bool cflag = false;
	TmpNastrMap=nmap;
	std::cout << '\n';
	while (1) {//main While begin
		std::cout << "\nChoose action:\n for quit press 'q' \n for display settings press 's' \n for change 'key = value' press 'c' \n for create new 'key = value' press 'n' \n for delete 'key = value' press 'd' \n";
		std::cin >> d;
		if (d == "q") { break; }
		if (d == "s") {
			std::cout << '\n';
			for (auto s : TmpNastrMap) {
				std::cout << s.first << " = " << s.second << '\n';
			}
		}
		if (d == "c") {//c begin
			std::cout << '\n';
			while (1) {
				std::cout << "Insert name of key for CHANGING (or 'q' for exit )\n ";
				std::cin >> d;
				if (d == "q") { break; }
				if (TmpNastrMap.find(d) == TmpNastrMap.end()) { std::cout << "No such key! Changing abort!\n"; }
				else {
					std::cout << "Insert new value of key " + d + "\n ";
					std::cin >> nv;
					std::cout << "Rewrite key? (y/n) \n ";
					std::cin >> j;
					if (j == "y") { TmpNastrMap.insert_or_assign(d, nv); cflag = true; }
				}
			}
			d = "";
		}//c end;
		if (d == "n") {//n begin
			std::cout << '\n';
			while (1) {
				std::cout << "Insert name of NEW key (or 'q' for exit )\n ";
				std::cin >> d;
				if (d == "q") { break; }
				std::cout << "Insert value of NEW key\n ";
				std::cin >> nv;
				if (TmpNastrMap.find(d) != TmpNastrMap.end()) { std::cout << "WARNING! KEY " + d + " ALREADY EXISTS! Creation abort!\n"; }
				else {
					std::cout << "Create new key = value? " + d + "=" + nv + " (y/n) \n ";
					std::cin >> j;
					if (j == "y") { TmpNastrMap.insert_or_assign(d, nv); cflag = true; }
				}

			}
		}//n end
		if (d == "d") {//d begin
			std::cout << '\n';
			while (1) {
				std::cout << "Insert name of key to DELETE (or 'q' for exit )\n ";
				std::cin >> d;
				if (d == "q") { break; }
				if (TmpNastrMap.find(d) == TmpNastrMap.end()) { std::cout << "No such key! Deleting abort!\n"; }
				else {
					std::cout << "Delete key = value? " + d + " (y/n) \n ";
					std::cin >> j;
					if (j == "y") { TmpNastrMap.erase(TmpNastrMap.find(d)); cflag = true; }
				}
			}
		}//d end;
	}//main While end
		if (cflag) {
			std::cout << "\n Save all changes? (y/n) \n ";
			std::cin >> j;
			if (j == "y") {
				nmap.swap(TmpNastrMap);
			}

		}

		SaveLoadParam(SettFile, 0);
	
}

errorMsg::errorMsg(std::string mss)
{
	ErrMsg = mss;
}

errorMsg::~errorMsg()
{
	ErrMsg = "";
}
