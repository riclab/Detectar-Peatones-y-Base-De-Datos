#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    int ini =0;
    thread1.setMessage(ini);

}

MainWindow::~MainWindow()
{
    thread1.stop();
    thread1.wait();
    delete ui;
}

/**
 * Metodo para obtener la direccion u origen    1del video
 * @brief MainWindow::SeleccionarVideo
 */
void MainWindow::SeleccionarVideo(){
    // Declara la variable con la ruta del archivo
    QString archivo = QFileDialog::getOpenFileName(this, tr("Abrir Video"),
                                                    "",
                                                    tr("Videos (*.avi *.mp4 *.mov *.m4v)"));
    //Agrega la ruta del archivo
    ui->labelVideo->setText(archivo);
    ui->radioVideo->setChecked(true);
}


/**
 * Metodo para procesar el video frame a frame si ckecked==true
 * @brief MainWindow::ProcesarVideo
 * @param checked
 *
 */
void MainWindow::ProcesarVideo(bool checked){

    cv::destroyAllWindows(); // Para cerrar todas las ventanas

    unsigned long Atime;
    int fps;

    cv::Ptr< cv::cuda::HOG> d_hog = cv::cuda::HOG::create(cv::Size(48, 96)); //Size(64, 128));//
    d_hog->setSVMDetector(d_hog->getDefaultPeopleDetector());

    cv::Mat frame; // Frame como array multidimencional

    QTime time;


    if(!checked){ // Si !checked detiene el video si no lo procesa
        ui->play->setText("Iniciar video");
        cap.release();
    }else{
        ui->play->setText("Parar video");

        if(ui->radioVideo->isChecked()){ // si el "radio button" esta seleccionado ejecuta el video si no la webcam
            cap.open(ui->labelVideo->text().toStdString().c_str());
            ui->label_muestra_origen->setText("Video Pre-Grabado");
        }else{
            cap.open(0);
            ui->label_muestra_origen->setText("Video en Vivo");

        }

        cap >> frame;


        //cambiar el tamaño
        double scale = float(800) / frame.cols;
        cv::cuda::GpuMat GpuImg, rGpuImg;
        GpuImg.upload(frame);
        cv::cuda::resize(GpuImg, rGpuImg, cv::Size(GpuImg.cols * scale, GpuImg.rows * scale));
        cv::Mat rInimg;
        rGpuImg.download(rInimg);

        int peatones;

        time.start();

        while(1) // bucle hasta que se precione "parar video"
        {
            Atime = cv::getTickCount();//tiempo inicial

            cap >> frame; // obtiene un nuevo frame del video o camara

            if(frame.empty()) break; // detiene el bucle si elframe esta vacio

            //cambiar el tamaño
            GpuImg.upload(frame);
            cv::cuda::resize(GpuImg, rGpuImg, cv::Size(GpuImg.cols * scale, GpuImg.rows * scale));
            rGpuImg.download(rInimg);
            cv::cuda::cvtColor(rGpuImg, rGpuImg, CV_BGR2GRAY);


            std::vector< cv::Point> found_locations;
            d_hog->detect(rGpuImg, found_locations);

            std::vector< cv::Rect> found_locations_rect;
            d_hog->detectMultiScale(rGpuImg, found_locations_rect);

            std::vector<cv::Rect> resRects;
            nms(found_locations_rect, resRects, 0.1f);

            for (unsigned int i = 0; i < resRects.size(); ++i)
            {
             cv::Rect peaton_i = resRects[i];
             cv::rectangle(rInimg, peaton_i, CV_RGB(0, 255,0), 2);

             int pos_x = std::max(peaton_i.tl().x - 10, 0);
             int pos_y = std::max(peaton_i.tl().y - 10, 0);
             cv::putText(rInimg, "Peaton", cv::Point(pos_x, pos_y), CV_FONT_HERSHEY_DUPLEX, 0.8 , CV_RGB(0,255,0), 1.5);

            }

            //guarda datos en la bd cada 2segundos
            if(time.elapsed()>2000){

                peatones = resRects.size();
                thread1.setMessage(peatones);

                if (thread1.isRunning())
                {
                    thread1.stop();
                }else{
                    thread1.start();
                }

                time.restart();
            }

            cv::namedWindow("Reproductor", cv::WINDOW_KEEPRATIO); // creamos una ventana la cual permita redimencionar
            cv::imshow("Reproductor", rInimg); // se muestran los frames

            fps = cv::getTickFrequency()/(cv::getTickCount() - Atime);

            ui->label_muestra_fps->setText(QString::number(fps));
            ui->label_muestra_nro->setText(QString::number(resRects.size()));

            char key = (char) cv::waitKey(20); //espera 20ms por la tecla ESC
            if(key == 27) break; //detiene el bucle
        }

    }

}


/**
 * Metodo para aplicar la Non-Maximum Suppression, borrando detecciones duplicadas
 * @brief nms
 * @param srcRects
 * @param resRects
 * @param thresh
 */
void MainWindow::nms(const std::vector<cv::Rect>& srcRects, std::vector<cv::Rect>& resRects, float thresh)
{

    resRects.clear();
    const size_t size = srcRects.size();
    if (!size){
        return;
    }

    // Ordena los cuadros de límite desde la parte inferior (eje y derecho)
    std::multimap<int, size_t> idxs;
    for (size_t i = 0; i < size; ++i)
    {
        idxs.insert(std::pair<int, size_t>(srcRects[i].br().y, i));
    }

    // mantendra el bucle mientras los índices siguen en la lista
    while (idxs.size() > 0){
        // obtiene el último rectángulo
        auto lastElem = --std::end(idxs);
        const cv::Rect& rect1 = srcRects[lastElem->second];
        resRects.push_back(rect1);
        idxs.erase(lastElem);

        for (auto pos = std::begin(idxs); pos != std::end(idxs); ) {
            // obtiene el rectángulo actual
            const cv::Rect& rect2 = srcRects[pos->second];
            float intArea = (rect1 & rect2).area();
            float unionArea = rect1.area() + rect2.area() - intArea;
            float overlap = intArea / unionArea;

            // si hay superposición, suprime el cuadro de actual
            if (overlap > thresh){
                pos = idxs.erase(pos);
            }else{
                ++pos;
            }
        }
    }
}


void MainWindow::on_toolButton_clicked()
{
    SeleccionarVideo();
}

void MainWindow::on_actionAbrir_Video_triggered()
{
    SeleccionarVideo();
}


void MainWindow::on_play_toggled(bool checked)
{
    ProcesarVideo(checked);
}
