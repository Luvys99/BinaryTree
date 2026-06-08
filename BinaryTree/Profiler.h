#pragma once

#include <windows.h>
#include <cstring>
#include <iostream>

#ifndef PROFILE
// 매크로 "\"뒤에 공백들어가면 오류 바로 "\n"해야됨
#define PROFILE \
	Profiler _p(__FUNCTIONW__);
#endif

struct ProfileData
{
	long _flag = 0; // 배열에 등록된 함수
	const wchar_t* _name = nullptr;
	double _totaltime = 0.0;
	double _Min = 0;
	double _Max = 0;
	long _callcount = 0;

};

// 전역 배열 선언
ProfileData list[30];
int arridx = 0;

class Profiler
{
public:
	Profiler() {}
	Profiler(const wchar_t* funcname) : tag(funcname)
	{
		Profiler_begin(tag);

	}
	~Profiler()
	{
		Profiler_end(tag);
		WriteProfile(L"ProfileReport.txt");
	}

	inline void Profiler_begin(const wchar_t* funcname) noexcept;
	inline void Profiler_end(const wchar_t* funcname) noexcept;

	static LARGE_INTEGER GetInitFrequency()
	{
		LARGE_INTEGER Freq;
		QueryPerformanceFrequency(&Freq);
		return Freq;
	}

	// 이름을 받아서 인덱스를 반환하는 함수
	inline int GetProfileID(const wchar_t* funcname) noexcept;
	// 함수 이름 인덱스로 등록하는 함수
	inline void RegisterFuncName(const wchar_t* funcname) noexcept;
	// 실행시간 측정한 데이터 저장
	inline void SaveData(double time, int idx) noexcept;
	// 파일 오픈 함수
	inline FILE* OpenFile(const wchar_t* filename) noexcept;
	// 리포트 타이틀그리는 함수
	inline void DrawProfileReportTitle(FILE* fp) const noexcept;
	// 리포트 그리는 함수
	inline void DrawProfileReport(FILE* fp) const noexcept;
	// 엔터누르면 한 번 호출하는 함수라 가독성 최우선
	inline void WriteProfile(const wchar_t* filename) noexcept;
	// 초기화 함수 ( 올바른 측정을 위한 )
	inline void ResetProfile() noexcept;
private:
	// Frequency 초기화 함수를 호출해서 바로 .cpp가 아닌 .h에서 초기화
	inline static LARGE_INTEGER Freq = GetInitFrequency();
	inline static int arridx = 0;
	LARGE_INTEGER Start;
	LARGE_INTEGER End;
	const wchar_t* tag;
	int _profileID;
	double _time = 0;

	// 구조체 내의 중요한 데이터기 때문에 함수 내부에서만 접근
	inline void UpdataTimeData(int idx, double time) noexcept;
	inline void UpdateMinMax(int idx, double time) noexcept;

};

inline void Profiler::Profiler_begin(const wchar_t* funcname) noexcept
{
	// 이미 등록한 함수인지 확인
	int idx = GetProfileID(funcname);

	// 등록안된 함수라면 등록
	if (idx == -1)
	{
		RegisterFuncName(funcname);
	}

	// 측정 시작
	QueryPerformanceCounter(&Start);

}

inline void Profiler::Profiler_end(const wchar_t* funcname) noexcept
{
	// 측정 종료 ( 함수 하나의 시간 측정 )
	QueryPerformanceCounter(&End);
	double deltatime = (double)(End.QuadPart - Start.QuadPart) / Freq.QuadPart;

	// idx 찾아서 거기에 시간 추가
	int currentidx = GetProfileID(funcname);
	SaveData(deltatime, currentidx); // 측정 시간 저장
}


inline void Profiler::UpdataTimeData(int idx, double time) noexcept
{
	list[idx]._totaltime += time;
	list[idx]._callcount++;
}

inline void Profiler::UpdateMinMax(int idx, double time) noexcept
{
	if (time < list[idx]._Min)
	{
		list[idx]._Min = time;
	}
	else if (time > list[idx]._Max)
	{
		list[idx]._Max = time;
	}
}

inline int Profiler::GetProfileID(const wchar_t* funcname) noexcept
{
	// 이미 등록된 이름이면 인덱스 반환
	for (int idx = 0; idx < arridx; idx++)
	{
		//strcpy_s 같은 복사비용 늘어나는 함수 금지, if문으로 묶을 수 있으면 한번에 묶어서 cmp명령어 줄이기
		if (list[idx]._flag == 1 && list[idx]._name == funcname)
		{
			return idx;
		}
	}

	// ID가 존재하지 않는다면 return
	return -1;
	
}

inline void Profiler::RegisterFuncName(const wchar_t* funcname) noexcept
{
	// arridx 값은 등록할 때만 ++ 되니까 굳이 검사 하지 않고 바로 등록
	// 예외로 30개까지가 끝인데 0 이하거나 30이상인 경우 리턴
	if (arridx < 0 || arridx >= 30) return;

	list[arridx]._flag = 1;
	list[arridx]._name = funcname;
	list[arridx]._Max = 0;
	list[arridx]._Min = 999999.0; //왜 999999.0 설정했지?
	list[arridx]._totaltime = 0.0;
	list[arridx]._callcount = 0;

	arridx++;
}

inline void Profiler::SaveData(double time, int idx) noexcept
{
	if (idx < 0 || idx >= 30) return;

	// 시간 업데이트, Min, Max 업데이트 함수 분리
	UpdataTimeData(idx, time);
	UpdateMinMax(idx, time);
}

inline FILE* Profiler::OpenFile(const wchar_t* filename) noexcept
{
	FILE* fp = nullptr;
	//fopen_s 반환값은 성공하면 0, 에러가 발생하면 에러코드(errno_t)
	errno_t err = _wfopen_s(&fp, filename, L"w");
	if (err == 0)
	{
		std::cout << "File Open Success!!\n";
		return fp;
	}
	else
	{
		std::cout << "File Open Failed!! Error Code : "<< err << "\n";
		return nullptr;
	}

}

inline void Profiler::DrawProfileReportTitle(FILE* fp) const noexcept
{
	fwprintf(fp, L"\t----------------------------------------------------------------------------------------------\n\n");
	fwprintf(fp, L"\t| Name\t|\tAverage\t\t|\tMin\t\t|\tMax\t\t|\tCall\t|\n");
	fwprintf(fp, L"\t----------------------------------------------------------------------------------------------\n");
}

inline void Profiler::DrawProfileReport(FILE* fp) const noexcept
{
	for (int idx = 0; idx < arridx; idx++)
	{
		double averageTime = (list[idx]._callcount > 0) ? (list[idx]._totaltime / list[idx]._callcount) : 0.0;

		fwprintf(fp, L"\t| %ls  |  %lfs\t|  %lfs\t|  %lfs\t|\t%d\n", list[idx]._name, averageTime,
			list[idx]._Min, list[idx]._Max, list[idx]._callcount);
	}
	fwprintf(fp, L"\t---------------------------------------------------------------------------------------------\n");
}

inline void Profiler::WriteProfile(const wchar_t* filename) noexcept
{
	FILE* fp = OpenFile(filename);

	DrawProfileReportTitle(fp);
	DrawProfileReport(fp);
	fclose(fp);
}

inline void Profiler::ResetProfile() noexcept
{
	for (int idx = 0; idx < arridx; idx++)
	{
		// 전체를 초기화 할 필요 없이 등록했던 것만 초기화
		list[idx]._callcount = 0;
		list[idx]._Max = 0.0;;
		list[idx]._Min = 999999.0;
		list[idx]._totaltime = 0.0;
	}
}
