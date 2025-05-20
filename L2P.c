#include <cvirte.h>		
#include <userint.h>
#include <utility.h>
#include <formatio.h>
#include <ansi_c.h>		
#include "L2P.h"
#include <analysis.h>

#define SAMPLE_RATE		0
#define NPOINTS			1

static int panelHandle;
int waveInfo[2]; //waveInfo[0] = sampleRate
				 //waveInfo[1] = number of elements
double sampleRate = 0.0;
int npoints = 0;
double *waveData = 0;
int startIndex = 0;
const int esantionSize = 256;
//double *filtru = 0;


int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "L2P.uir", MAIN_PANEL)) < 0)
		return -1;
	DisplayPanel (panelHandle);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	return 0;
}

void filtrareAlpha(double* waveData, double* filt, int n, double alpha){
	filt[0] = waveData[0];  // prima valoare este aceeasi ca in waveData
    
    for (int i = 1; i < n; i++) {
        filt[i] = (1 - alpha) * filt[i - 1] + alpha * waveData[i];
    }

    //salvam in fisier
    ArrayToFile("filteredDataAlpha.txt", filt, VAL_DOUBLE, n, 2, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_SEP_BY_TAB, VAL_CONST_WIDTH, VAL_ASCII, VAL_TRUNCATE);		
}


void filtrareMediere(double* waveData, double* filt, int n)
{
	int contor = 0;
	for(int i=0;i<npoints/16;i++){
			for(int j=contor;j<contor+16;j++){
				filt[i]+=waveData[j];
			}
			filt[i]/=16;
			contor+=16;
		}
}

double calculMedie(double* data, int n){
	if (n <= 0) return 0; //evitam impartire la 0

    double sum = 0;
    for (int i = 0; i < n; i++) {
        sum += data[i];
    }
    return sum / n;
}

double calculDispersie(double* data, int n){
	 if (n <= 1) return 0; // fara impartire la 0 sau vect de date gol
	
	 //avem nevoie de media datelor, calculata anterior in functia calculMedie
	 //variatia reprezinta suma diferentelor fiecarei date, ridicate la patrat
	 //impartim la n ca sa obtinem dispersia
    double mean = calculMedie(data, n);
    double variatie = 0;
    for (int i = 0; i < n; i++) {
        variatie += (data[i] - mean) * (data[i] - mean);
    }
    return variatie / (n-1);
}


void calculSkewnessKurtosis(double* waveData, int startIndex, int n, double* skewness, double* kurtosis) {
    int numPoints = (n < 256) ? n : 256;
    if (numPoints <= 0) {
        *skewness = 0.0;
        *kurtosis = 0.0;
        return;
    }
    // Calcul?m varianta
    double variance = calculDispersie(waveData + startIndex, numPoints);

    // Momentul de ordinul 3 (skewness)
    Moment(waveData + startIndex, numPoints, 3, skewness);
    // Momentul de ordinul 4 (kurtosis)
    Moment(waveData + startIndex, numPoints, 4, kurtosis);

    // Normaliz?m pentru skewness ?i kurtosis
    *skewness /= (variance * sqrt(variance));  // Skewness = M3 / (sigma^3)
    *kurtosis /= (variance * variance);        // Kurtosis = M4 / (sigma^4)
}



int CVICALLBACK OnMainPanel (int panel, int event, void *callbackData,
							 int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface(0);

			break;
	}
	return 0;
}

void calculMinMax(double* data, int n, double* min, double* max, int* minIndex, int* maxIndex){
	if (n <= 0) return; //vector de date gol --> return

    *min = data[0];
    *max = data[0];

    for (int i = 1; i < n; i++) {
        if (data[i] < *min) {
            *min = data[i];
			*minIndex = i;
        }
        if (data[i] > *max) {
            *max = data[i];
			*maxIndex = i;
        }
    }
}

void detectareMaximeMinimeLocale(double* data, int n, double* maxima, int* maxIndici, int* nrMaxime, double* minima, int* minIndici, int* nrMinime) {
    *nrMaxime = 0;
    *nrMinime = 0;

    for (int i = 1; i < n - 1; i++) {
        if (data[i] > data[i - 1] && data[i] > data[i + 1]) {
            maxima[*nrMaxime] = data[i];
            maxIndici[*nrMaxime] = i;
            (*nrMaxime)++;
        } else if (data[i] < data[i - 1] && data[i] < data[i + 1]) {
            minima[*nrMinime] = data[i];
            minIndici[*nrMinime] = i;
            (*nrMinime)++;
        }
    }
}

void proceseazaInterval(int panel) {
    double skewness = 0.0, kurtosis = 0.0;
	int filterType; //0 pt mediere, 1 pt alpha
	double alpha = 0.05;

    //calculam skewness si kurtosis pt intervalul curent
    calculSkewnessKurtosis(waveData, startIndex, sampleRate, &skewness, &kurtosis);

    //afisare pe butoane actualizata
    SetCtrlVal(panel, MAIN_PANEL_BUTTON_SKEWNESS, skewness);
    SetCtrlVal(panel, MAIN_PANEL_BUTTON_KURTOSIS, kurtosis);

    //grafic original
    DeleteGraphPlot(panel, MAIN_PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
    PlotY(panel, MAIN_PANEL_GRAPH, waveData + startIndex, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
	
	//grafic semnal filtrat
    
	GetCtrlVal(panel, MAIN_PANEL_BTT_FILTRU, &filterType);
			
	if(filterType==0){
		double * filt = (double*)calloc(npoints/16,sizeof(double));
		filtrareMediere(waveData,filt, npoints);
		DeleteGraphPlot(panel, MAIN_PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
		PlotY(panel, MAIN_PANEL_GRAPH_2, filt + startIndex/16, sampleRate/16, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
	}
	else if(filterType==1){
		double * filt = (double*)calloc(npoints,sizeof(double));
		GetCtrlVal(panel, MAIN_PANEL_BTT_ALPHA, &alpha);
		filtrareAlpha(waveData,filt, npoints, alpha);
		DeleteGraphPlot(panel, MAIN_PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
		PlotY(panel, MAIN_PANEL_GRAPH_2, filt + startIndex, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
	}
			
			
	//afisare tabel filtrat
	

    //salvez grafic
    char fileName[256];
    int bitmapID;
    sprintf(fileName, "grafic_original_%d.jpg", startIndex / esantionSize);
    GetCtrlDisplayBitmap(panel, MAIN_PANEL_GRAPH, 1, &bitmapID);
    SaveBitmapToJPEGFile(bitmapID, fileName, JPEG_PROGRESSIVE, 100);

    sprintf(fileName, "grafic_filtrat_%d.jpg", startIndex / esantionSize);
    GetCtrlDisplayBitmap(panel, MAIN_PANEL_GRAPH_2, 1, &bitmapID);
    SaveBitmapToJPEGFile(bitmapID, fileName, JPEG_PROGRESSIVE, 100);

	
    

}



int treceriZero(double* data, int n){
	int zeros = 0;

    for (int i = 1; i < npoints; i++) {
        //verificam trecere de la pozitiv la negativ SAU invers
        if ((waveData[i-1] > 0 && waveData[i] < 0) || (waveData[i-1] < 0 && waveData[i] > 0)) {
            zeros++;
        }
    }

    return zeros;
}

int comparare(const void* a, const void * b){
return (*(double*)a - *(double*)b);  // fct de comparare pt qsort
}

double calculareMediana(double *signal, int npoints) {
    // Copierea semnalului într-un vector temporar pentru sortare
    double *temp = (double *)malloc(npoints * sizeof(double));
    if (!temp) {
        fprintf(stderr, "Eroare: Alocare memorie esuat?.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < npoints; i++) {
        temp[i] = signal[i];
    }

    // Sortarea vectorului
    qsort(temp, npoints, sizeof(double), comparare);

    // Calcularea medianei
    double median;
    if (npoints % 2 == 0) {
        // Num?r par de elemente
        median = (temp[npoints / 2 - 1] + temp[npoints / 2]) / 2.0;
    } else {
        // Num?r impar de elemente
        median = temp[npoints / 2];
    }

    // Eliberarea memoriei
    free(temp);

    return median;
}


int CVICALLBACK OnLoad (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	double min, max, medie, dispersie, mediana, alpha = 0.05;
	double skewness = 0.0, kurtosis = 0.0;
	int minIndex, maxIndex, zeros;
	int filterType; //0 pt mediere, 1 pt alpha

	switch (event)
	{
		case EVENT_COMMIT:
			//exec script py
			LaunchExecutable("python wavtotext.py");
			
			//astept sa fie generate cele doua fisiere (modificati timpul daca este necesar)
			Delay(4);
			
			//incarc informatiile privind rata de esantionare si numarul de valori
			FileToArray("wafeInfo.txt", waveInfo, VAL_INTEGER, 2, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			sampleRate = waveInfo[SAMPLE_RATE];
			npoints = waveInfo[NPOINTS];
			
			//memorie nr puncte
			waveData = (double *) calloc(npoints, sizeof(double));
			
			
			//incarcare .txt in memorie
			FileToArray("waveData.txt", waveData, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			
			// initializam primul interval
			
			 startIndex = 0;  //indexul de start este primul interval
           // proceseazaInterval(panel);  //procesam primul interval
			
			//afisare tabel nefiltrat
			DeleteGraphPlot(panel, MAIN_PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, MAIN_PANEL_GRAPH, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
			//filtrare
			DeleteGraphPlot(panel, MAIN_PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
			GetCtrlVal(panel, MAIN_PANEL_BTT_FILTRU, &filterType);
			double* filt;
			if(filterType==0){
				filt = (double*)calloc(npoints/16,sizeof(double));
				filtrareMediere(waveData,filt, npoints/16);
				//afisare tabel filtrat
				PlotY(panel, MAIN_PANEL_GRAPH_2, filt, npoints/16, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
			}
			else if(filterType==1){
				filt = (double*)calloc(npoints,sizeof(double));
				GetCtrlVal(panel, MAIN_PANEL_BTT_ALPHA, &alpha);
				filtrareAlpha(waveData,filt, npoints, alpha);
				//afisare tabel filtrat
				PlotY(panel, MAIN_PANEL_GRAPH_2, filt, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
			}
			
			
			
			
			
			//calcule pt min max medie dispersie etc
			
			calculMinMax(waveData, npoints, &min, &max, &minIndex, &maxIndex);
			medie = calculMedie(waveData, npoints);
			dispersie = calculDispersie(waveData, npoints);
			zeros = treceriZero(waveData, npoints);
			mediana = calculareMediana(waveData, npoints);
			calculSkewnessKurtosis(waveData, startIndex, esantionSize, &skewness, &kurtosis);
			
			
			//afisare pe butoane
			SetCtrlVal(panel, MAIN_PANEL_BUTTON_MIN, min);
			SetCtrlVal(panel, MAIN_PANEL_BUTTON_MAX, max);
			SetCtrlVal(panel, MAIN_PANEL_BUTTON_MININDEX, minIndex);
			SetCtrlVal(panel, MAIN_PANEL_BUTTON_MAXINDEX, maxIndex);
			SetCtrlVal(panel, MAIN_PANEL_BUTTON_MEDIE, medie);
			SetCtrlVal(panel, MAIN_PANEL_BUTTON_DISPERSIE, dispersie);
			SetCtrlVal(panel, MAIN_PANEL_BUTTON_ZERO, zeros);
			SetCtrlVal(panel, MAIN_PANEL_BUTTON_MEDIANA, mediana);
			SetCtrlVal(panel, MAIN_PANEL_BUTTON_SKEWNESS, skewness);
			SetCtrlVal(panel, MAIN_PANEL_BUTTON_KURTOSIS, kurtosis);
		

			// Calcularea histogramei
			int numBins = npoints; // numarul de intervale (bins)
			double base = min; // limita inferioara
			double top = max;  // limita superioara
			
			// Alocare memorie pentru histograma si axele
			ssize_t* histogramArray = (ssize_t*)calloc(numBins+1, sizeof(ssize_t)); 
			double* axisArray = (double*)calloc(numBins + 1, sizeof(double)); 

			// Apelarea functiei Histogram
			Histogram(waveData, npoints, base, top, histogramArray, axisArray, numBins);

			// Plotarea histogramei
			PlotXY(panel, MAIN_PANEL_GRAPH_HISTOGRAM, axisArray, histogramArray, numBins + 1, VAL_DOUBLE, VAL_INTEGER, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);
			
			
			
			//calcul envelope cu transformata Hilbert
			//double* envelope = (double*)calloc(npoints, sizeof(double));
			
			
			// Eliberare memorie
			free(filt);
			free(histogramArray);
			free(axisArray);
			
			break;

	
	}
	return 0;
}

int CVICALLBACK OnPrev (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			startIndex -= sampleRate; //mutam indexul la secunda sau esantionul anterior
			if (startIndex < 0)
				startIndex = 0; //evitam un index negativ
			
			proceseazaInterval(panel);
			
			break;
	}
	return 0;
}

int CVICALLBACK OnNext (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			startIndex += sampleRate; //mutam indexul la o secunda dupa
			if ( startIndex + sampleRate > waveInfo[1] )
				startIndex = npoints - sampleRate; // daca se ajunge la o fereastra mai mare de npoints, mergem la ultimul interval, care este npoints - marimea unui esantion ( 256 )
			
			proceseazaInterval(panel);
			
			break;
	}
	return 0;
}


int CVICALLBACK OnEnvelope (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			LaunchExecutable("python hilbert.py");
			double* countHilbert=(double*)calloc(2,sizeof(double));
			//astept sa fie generate cele doua fisiere (modificati timpul daca este necesar
			Delay(4);
			
			//incarc informatiile privind rata de esantionare si numarul de valori
			
			FileToArray("hilbert_count.txt", countHilbert, VAL_DOUBLE, 2, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			double* dataHilbert=(double*)calloc(countHilbert[0], sizeof(double));
			FileToArray("hilbert_results.txt", dataHilbert, VAL_DOUBLE, countHilbert[0], 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			
			DeleteGraphPlot(panel, MAIN_PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, MAIN_PANEL_GRAPH, dataHilbert, countHilbert[0], VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
			break;
	}
	return 0;
}

