#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <memory>
#include <cmath>

using namespace std;

class Gesture {
public:
    string name;
    vector<Point> points;

    Gesture(string name, vector<Point> points) : name(name), points(points) {}
};

class Parser {
public:
    map<string, map<string, map<string, vector<Gesture>>>> offlineData;

    void preProcessOfflineData() {
        for (auto& user : offlineData) {
            for (auto& speed : user.second) {
                for (auto& gesture : speed.second) {
                    vector<Gesture> preProcessedGestures;
                    for (auto& example : gesture.second) {
                        vector<Point> preProcessedPoints = getPreProcessPoints(example.points);
                        preProcessedGestures.emplace_back(example.name, preProcessedPoints);
                    }
                    gesture.second = preProcessedGestures;
                }
            }
        }
    }

    vector<Point> getPreProcessPoints(vector<Point> points) {
        // Preprocessing code here
        return points;
    }

    void readOfflineData() {
        // Reading offline data from file
    }
};

//we can import this from the already written code and use this here. 
//Or we can simply import the necessary code here and run the recognizer on the 
class Recognizer {
public:
    Recognizer(vector<Gesture> trainingSet) {
        // Training code here
    }

    tuple<string, float, float, vector<Gesture>> recognizeGesture(vector<Point> points) {
        // Recognition code here
        return make_tuple("", 0, 0, vector<Gesture>{});
    }
};

class OfflineRecognizer {
private:
    Parser parser;
    Recognizer recognizer;
    std::map<std::string, std::map<std::string, std::map<std::string, std::vector<std::vector<Point>>>>> offlineData;
    std::map<std::string, std::map<std::string, std::map<std::string, std::vector<std::vector<Point>>>>> preProcessedData;

    void preProcessOfflineData() {
        preProcessedData = offlineData;
        for (auto& user : offlineData) {
            for (auto& speed : user.second) {
                for (auto& gesture : speed.second) {
                    preProcessedData[user.first][speed.first][gesture.first] = std::vector<std::vector<Point>>();
                    for (auto& points : gesture.second) {
                        preProcessedData[user.first][speed.first][gesture.first].push_back(recognizer.getPreProcessPoints(points));
                    }
                }
            }
        }
    }

    void recognizeOfflineData() {
        std::map<std::string, std::map<int, std::map<std::string, int>>> score;
        for (auto& user : preProcessedData) {
            score[user.first] = std::map<int, std::map<std::string, int>>();
            for (int example = 1; example < 2; example++) {
                score[user.first][example] = std::map<std::string, int>();
                for (int i = 1; i < 2; i++) {
                    auto [training_set, testing_set] = getSplitData(preProcessedData[user.first]["medium"], example);
                    Recognizer recognizer(training_set);
                    for (auto& gesture : testing_set) {
                        if (score[user.first][example].find(gesture.first) == score[user.first][example].end()) {
                            score[user.first][example][gesture.first] = 0;
                        }
                        auto [recognizedGesture, _, _, Nbest] = recognizer.recognizeGesture(gesture.second);
                        recognizedGesture = recognizedGesture.find("/") != std::string::npos ? recognizedGesture.substr(0, recognizedGesture.find("/")) : "";
                        if (recognizedGesture == gesture.first) {
                            score[user.first][example][gesture.first] += 1;
                        }
                    }
                }
            }
        }
        writeToFile(dumps(score), "score.json");
    }

    std::pair<std::map<std::string, std::vector<std::vector<Point>>>, std::map<std::string, std::vector<Point>>> getSplitData(std::map<std::string, std::vector<std::vector<Point>>> gestures, int E) {
        std::map<std::string, std::vector<std::vector<Point>>> training_set;
        std::map<std::string, std::vector<Point>> testing_set;
        std::srand(std::time(nullptr));
        for (auto& gesture : gestures) {
            for (int i = 0; i < E; i++) {
                training_set[gesture.first + "/" + std::to_string(i)] = gesture.second[i];
            }
            testing_set[gesture.first] = gesture.second[std::rand() % (10 - E) + E];
        }
        return std::make_pair(training_set, testing_set);
    }
