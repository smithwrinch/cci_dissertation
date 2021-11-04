#include "../baseScene.h"
#include "../sceneManager.h"
#include "../model.h"
#include "../sceneTypes.h"

#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <filesystem>

static pid_t system_(const char * command, int * infp, int * outfp)
{
    int p_stdin[2];
    int p_stdout[2];
    pid_t pid;

    if (pipe(p_stdin) == -1)
        return -1;

    if (pipe(p_stdout) == -1) {
        close(p_stdin[0]);
        close(p_stdin[1]);
        return -1;
    }

    pid = fork();

    if (pid < 0) {
        close(p_stdin[0]);
        close(p_stdin[1]);
        close(p_stdout[0]);
        close(p_stdout[1]);
        return pid;
    } else if (pid == 0) {
        close(p_stdin[1]);
        dup2(p_stdin[0], 0);
        close(p_stdout[0]);
        dup2(p_stdout[1], 1);
        dup2(::open("/dev/null", O_RDONLY), 2);
        /// Close all other descriptors for the safety sake.
        for (int i = 3; i < 4096; ++i)
            ::close(i);

        setsid();
        execl("/bin/sh", "sh", "-c", command, NULL);
        _exit(1);
    }

    close(p_stdin[0]);
    close(p_stdout[1]);

    if (infp == NULL) {
        close(p_stdin[1]);
    } else {
        *infp = p_stdin[1];
    }

    if (outfp == NULL) {
        close(p_stdout[0]);
    } else {
        *outfp = p_stdout[0];
    }

    return pid;
}

class PythonThread : public ofThread {
  public:
    string gan_dir;
    string txt_dir;
    string fname;
    string song_dir;
    int latent_dim;
    int fps;
    float speedFpm;
    float pulseReact;
    float motionReact;
    bool saveFrames;
    bool pulsePercussive;
    bool pulseHarmonic;
    bool motionPercussive;
    bool motionHarmonic;
    float motionRandomness;

    int pid;
    int input;
    int output;

    void setup(
      string gan_dir,
      string txt_dir,
      string fname,
      string song_dir,
      int latent_dim,
      int fps,
      float speedFpm,
      float pulseReact,
      float motionReact,
      bool saveFrames,
      bool pulsePercussive,
      bool pulseHarmonic,
      bool motionPercussive,
      bool motionHarmonic,
      float motionRandomness
    ){
      this->gan_dir = gan_dir;
      this->txt_dir = txt_dir;
      this->fname = fname;
      this->song_dir = song_dir;
      this->latent_dim = latent_dim;
      this->fps = fps;
      this->speedFpm = speedFpm;
      this->pulseReact = pulseReact;
      this->motionReact = motionReact;
      this->saveFrames = saveFrames;
      this->pulsePercussive = pulsePercussive;
      this->pulseHarmonic = pulseHarmonic;
      this->motionPercussive = motionPercussive;
      this->motionHarmonic = motionHarmonic;
      this->motionRandomness = motionRandomness;
    }

    void threadedFunction(){
      string pythonFile = "python data/lucidsonicdreams/main.py ";
      // system((pythonFile + getArguments()).c_str());
      pid = system_((pythonFile + getArguments()).c_str(), &input, &output);
      cout << "STARTED PROCESS: " << ofToString(pid) << endl;
    }

    void stopThread(){
      cout << "STOPPING PYTHON THREAD:" << endl;
			kill(pid, SIGTERM);
			sleep(3000);
			kill(pid, SIGKILL);
			system("killall python"); // hacky workaround for now. just POSIX.
			system("killall python3");
			ofThread::stopThread();
		}

  private:
    string getArguments(){
      string out = "";
      out += " --fname '" + fname+"'";
      out += " --gan_dir '" + gan_dir +"'";
      out += " --song_dir '" + song_dir+"'";
      out += " --txt_dir '" + txt_dir+"'";
      out += " --latent_dim " + ofToString(latent_dim);
      out += " --speed_fpm " + ofToString(speedFpm);
      out += " --pulse_react " + ofToString(pulseReact);
      out += " --motion_react " + ofToString(motionReact);
      out += " --pulse_percussive " + ofToString(pulsePercussive);
      out += " --pulse_harmonic " + ofToString(pulseHarmonic);
      out += " --motion_percussive " + ofToString(motionPercussive);
      out += " --motion_harmonic " + ofToString(motionHarmonic);
      out += " --motion_randomness " + ofToString(motionRandomness);
      out += " --save_frames " + ofToString(saveFrames);
      out += " --fps " + ofToString(fps);

      cout << out << endl;
      return out;
    }
};

class StatusThread : public ofThread {
  public:
    void setup(string fname, string * status, bool * finished){
      this->fname = fname;
      this->status = status;
      this->finished = finished;
      *finished = false;
    }

    void threadedFunction(){
      while(isThreadRunning()){
        ofFile file;
        if(file.open(fname)){
          buff = file.readToBuffer();
          cout << buff.getText() << endl;
          *status = buff.getText();
          if(buff.getText() == "Finished!"){
            sleep(1000);
            *finished = true;
          }
        }
        sleep(1000);
        file.close();
        sleep(1000);
      }
    }
    void stopThread(){
      cout << "STOPPING THREAD:" << endl;
      ofThread::stopThread();
      // file.close();
    }
  private:
    ofBuffer buff;
    string * status;
    string fname;
    bool * finished;

  };

class SyncMusicScene : public BaseScene {
  public:
    void setup();
    void update();
    void draw();
    void refresh();
    void onButtonEvent(ofxDatGuiButtonEvent e);

  private:

    void setupPythonThread();
    bool checkNameValid();

    ofxDatGuiButton * stopPython;

    ofxDatGuiSlider * speedFpmSlider;
    ofxDatGuiSlider * pulseReactSlider;
    ofxDatGuiSlider * motionReactSlider;

    ofxDatGuiButton * showAdvancedButton;


    ofxDatGuiToggle * pulsePercussiveToggle;
    ofxDatGuiToggle * pulseHarmonicToggle;
    ofxDatGuiButton * pulseSeperateButton;
    ofxDatGuiToggle * motionPercussiveToggle;
    ofxDatGuiToggle * motionHarmonicToggle;
    ofxDatGuiButton * motionSeperateButton;
    ofxDatGuiSlider * motionRandomnessSlider;
    ofxDatGuiToggle * saveFramesToggle;
    ofxDatGuiSlider * fpsSlider;


    ofxDatGuiButton * openMusicButton;
    ofxDatGuiButton * videoPathButton;
    ofxDatGuiLabel * errorLabel;
    ofxDatGuiTextInput * fnameInput;
    ofxDatGuiButton * createButton;

    ofxDatGuiButton * backButton;

    vector <ofxDatGuiComponent *> gui;

    vector <ofxDatGuiComponent *> advanced;

    int state = 0;
    bool showingAdvanced = false;
    bool running = false;
    string musicDir = "";
    string videoDir = "";
    string gan_dir;
    string txt_dir;

    string status;
    bool finished = false;

    PythonThread pythonThread;
    StatusThread statusThread;
};
