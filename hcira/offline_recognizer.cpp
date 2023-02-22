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

class Point {//Defining point struct 
public:
	double x;
	double y;
	Point(double x, double y) {
		this->x = x;
		this->y = y;
	}
};

struct Rect { //Defining rectangle for bounding box
	double X;
	double Y;
	double Width;
	double Height;

	Rect(double x, double y, double width, double height)
		: X(x), Y(y), Width(width), Height(height) {}
};

const int NumUnistrokes = 16;//Tempates loaded
const int NumPoints = 64;
const double SquareSize = 250.0;
const Point Origin = { 0, 0 };
const double Diagonal = sqrt(SquareSize * SquareSize + SquareSize * SquareSize);//Diagonal length
const double HalfDiagonal = 0.5 * Diagonal;
const double AngleRange = 45.0 * M_PI / 180.0;
const double AnglePrecision = 2.0 * M_PI / 180.0;
const double Phi = 0.5 * (-1.0 + sqrt(5.0));


struct Unistroke {//This is where preprocessing happens for points
public:
	 string name;
	 vector<Point> points;
	 vector<double> vectorizedPoints;


	Unistroke( string& name,  vector<Point>& points) //Constructor where the point are preprocessed and stored
	{
		//Note 1.Resample 2.Rotate By needs indicative angle 3.Scale 4.Transalate 5.For Protractor -- Vectorize
		this->name = name;
		this->points = Resample(points, NumPoints);//1
		double radians = IndicativeAngle(this->points);//2
		this->points = RotateBy(this->points, -radians);//3
		this->points = ScaleTo(this->points, SquareSize);//4
		this->points = TranslateTo(this->points, Origin);//5
		this->vectorizedPoints = Vectorize(this->points); // for Protractor	
	}
	 vector<Point>Resample( vector<Point>& points, int n)
	{

		double I = PathLength(points) / (n - 1); // interval length
		double D = 0.0;
		vector<Point> newpoints = { points[0] };
		for (int i = 1; i < points.size(); i++)
		{
			double d = Distance(points[i - 1], points[i]);
			if ((D + d) >= I)
			{
				double qx = points[i - 1].x + ((I - D) / d) * (points[i].x - points[i - 1].x);
				double qy = points[i - 1].y + ((I - D) / d) * (points[i].y - points[i - 1].y);
				Point q = Point(qx, qy);
				newpoints.push_back(q); // append new point 'q'
				points.insert(points.begin() + i, q); // insert 'q' at position i in points s.t. 'q' will be the next i
				D = 0.0;
			}
			else D += d;
		}
		if (newpoints.size() == n - 1) // somtimes we fall a rounding-error short of adding the last point, so add it if so
			newpoints.push_back(Point(points[points.size() - 1].x, points[points.size() - 1].y));
		return newpoints;
		//TODO Vikranth
	}
	double IndicativeAngle( vector<Point> points)
	{
		Point c = Centroid(points);
		return atan2(c.y - points[0].y, c.x - points[0].x);
	}

	 vector<Point> ScaleTo( vector<Point> points, double size)
	{
		Rect B = BoundingBox(points);
		 vector<Point> newpoints;
		for (int i = 0; i < points.size(); i++)
		{
			double qx = points[i].x * (size / B.Width);
			double qy = points[i].y * (size / B.Height);
			newpoints.push_back(Point(qx, qy));
		}//TODO Raghav
		return newpoints;
	}

	 vector<Point> TranslateTo(const  vector<Point>& points, Point pt)
	{
		Point c = Centroid(points);
		 vector<Point> newPoints;
		for (const auto& point : points)
		{
			Point q = { point.x + pt.x - c.x, point.y + pt.y - c.y };
			newPoints.push_back(q);
		}//TODO Vikranth
		return newPoints;

	}
	 vector<double> Vectorize(const  vector<Point>& points) {
		double sum = 0.0;
		 vector<double> vector;
		for (const auto& point : points) {
			vector.push_back(point.x);
			vector.push_back(point.y);
			sum += point.x * point.x + point.y * point.y;
		}//TODO Raghav
		double magnitude = sqrt(sum);
		for (auto& val : vector)
			val /= magnitude;
		return vector;
	}

	 vector<Point> RotateBy( vector<Point>& points, double radians) // rotates points around centroid
	{//TODO Vikranth
		Point c = Centroid(points);
		double cos =  cos(radians);
		double sin =  sin(radians);
		 vector<Point> newpoints;
		for (auto& point : points) {
			double qx = (point.x - c.x) * cos - (point.y - c.y) * sin + c.x;
			double qy = (point.x - c.x) * sin + (point.y - c.y) * cos + c.y;
			newpoints.emplace_back(qx, qy);
		}
		return newpoints;
	}


	Rect BoundingBox( vector<Point> points)
	{//TODO Vikranth
		double minX = INFINITY, maxX = -INFINITY, minY = INFINITY, maxY = -INFINITY;
		for (int i = 0; i < points.size(); i++) {
			minX = (double) min(minX, points[i].x);
			minY = (double) min(minY, points[i].y);
			maxX = (double) max(maxX, points[i].x);
			maxY = (double) max(maxY, points[i].y);
		}
		return Rect(minX, minY, maxX - minX, maxY - minY);
	}
	double PathLength( vector<Point> points)
	{
		double d = 0.0;
		for (int i = 1; i < points.size(); i++)
			d += Distance(points[i - 1], points[i]);
		return d;
	}
	double Distance(Point p1, Point p2)
	{
		double dx = p2.x - p1.x;
		double dy = p2.y - p1.y;
		return sqrt(dx * dx + dy * dy);
	}
	Point Centroid(const  vector<Point>& points)
	{
		double x = 0.0, y = 0.0;
		for (const auto& point : points) {
			x += point.x;
			y += point.y;
		}
		x /= points.size();
		y /= points.size();
		return { x, y };
	}




};

struct Result {//Result struct for displaying in canvas
	 string Name;
	double Score;
	int Time;

	Result(const  string& name, double score, int time) : Name(name), Score(score), Time(time) {}
};



class GestureRecognizer {

public:
	GestureRecognizer() {
		
	 vector<Unistroke> Unistrokes;


	Result Recognize(vector<Point>& points, bool useProtractor) {
		//TODO Raghav Vikranth
		auto t0 =  chrono::high_resolution_clock::now();
		string str = "";
		Unistroke candidate(str, points);//resampled here for candiate or input gesture

		int u = -1;//If nothing matches the initializaiton
		double b = INFINITY; //intMAX
		for (int i = 0; i < Unistrokes.size(); i++) {
			double d;

			if (useProtractor)//this is never invoked for now.
				d = OptimalCosineDistance(Unistrokes[i].vectorizedPoints, candidate.vectorizedPoints);
			else
				d = DistanceAtBestAngle(candidate.points, Unistrokes[i], -AngleRange, AngleRange, AnglePrecision);
			if (d < b) {
				b = d;
				u = i; //its going to point at the template that is under consideration and saves the value for the same for the min distance
			}
		}

		auto t1 =  chrono::high_resolution_clock::now();
		auto duration =  chrono::duration_cast< chrono::milliseconds>(t1 - t0).count();//Time elapsed
		return (u == -1) ? Result("No match.", 0.0, duration) :
			Result(Unistrokes[u].name, useProtractor ? (1.0 - b) : (1.0 - b / HalfDiagonal), duration);
	}


	double OptimalCosineDistance(const  vector<double>& v1, const  vector<double>& v2)
	{
		double a = 0.0;
		double b = 0.0;

		for (int i = 0; i < v1.size(); i += 2)
		{
			a += v1[i] * v2[i] + v1[i + 1] * v2[i + 1];
			b += v1[i] * v2[i + 1] - v1[i + 1] * v2[i];
		}

		double angle = atan(b / a);
		return acos(a * cos(angle) + b * sin(angle));
	}
	double DistanceAtBestAngle( vector<Point>& points, Unistroke& T, double a, double b, double threshold) {
		//TODO Vikranth
		double x1 = Phi * a + (1.0 - Phi) * b;
		double f1 = DistanceAtAngle(points, T, x1);
		double x2 = (1.0 - Phi) * a + Phi * b;
		double f2 = DistanceAtAngle(points, T, x2);
		while ( abs(b - a) > threshold) {
			if (f1 < f2) {
				b = x2;
				x2 = x1;
				f2 = f1;
				x1 = Phi * a + (1.0 - Phi) * b;
				f1 = DistanceAtAngle(points, T, x1);
			}
			else {
				a = x1;
				x1 = x2;
				f1 = f2;
				x2 = (1.0 - Phi) * a + Phi * b;
				f2 = DistanceAtAngle(points, T, x2);
			}
		}
		return  min(f1, f2);
	}
	double DistanceAtAngle( vector<Point>& points, Unistroke& T, double radians)
	{
		//TODO Raghav
		 vector<Point> newpoints = RotateBy(points, radians);

		return PathDistance(newpoints, T);
	}

	 vector<Point> RotateBy( vector<Point>& points, double radians) // rotates points around centroid
	{
		//TODO Vikranth
		Point c = Centroid(points);
		double cos =  cos(radians);
		double sin =  sin(radians);
		 vector<Point> newpoints;
		for (auto& point : points) {
			double qx = (point.x - c.x) * cos - (point.y - c.y) * sin + c.x;
			double qy = (point.x - c.x) * sin + (point.y - c.y) * cos + c.y;
			newpoints.emplace_back(qx, qy);
		}
		return newpoints;
	}

	double PathDistance(vector<Point> pts1, Unistroke inputStrokes)

	{//TODO Vikranth
		 vector<Point> pts2 = inputStrokes.points;//TODO 
		double d = 0.0;
		for (int i = 0; i < pts1.size(); i++) // assumes pts1.size() == pts2.size()
			d += Distance(pts1[i], pts2[i]);
		return d / pts1.size();
	}

	Point Centroid(const  vector<Point>& points)
	{
		//TODO Vikranth
		double x = 0.0, y = 0.0;
		for (const auto& point : points) {
			x += point.x;
			y += point.y;
		}
		x /= points.size();
		y /= points.size();
		return { x, y };
	}
	double Distance(Point p1, Point p2)
	{
		//TODO Vikranth
		double dx = p2.x - p1.x;
		double dy = p2.y - p1.y;
		return  sqrt(dx * dx + dy * dy);
	}

};


//class Gesture {
//public:
//    string name;
//    vector<Point> points;
//
//    Gesture(string name, vector<Point> points) : name(name), points(points) {}
//};


//we can import this from the already written code and use this here. 
//Or we can simply import the necessary code here and run the recognize

class OfflineRecognizer {
private:
	GestureRecognizer recognizer;
     map< string,  map< string,  map< string,  vector< vector<Point>>>>> offlineData;
     map< string,  map< string,  map< string,  vector< vector<Point>>>>> preProcessedData;

	void preProcessOfflineData() {
		preProcessedData = offlineData;
		for (auto& user : offlineData) {
			for (auto& speed : user.second) {
				if (speed.first == "medium") {
					for (auto& gesture : speed.second) {
						preProcessedData[user.first][speed.first][gesture.first] =  vector< vector<Point>>();
						for (auto& points : gesture.second) {
							Unistroke unistroke(gesture.first, points);
							preProcessedData[user.first][speed.first][gesture.first].push_back(unistroke.vectorizedPoints);
						}
					}
				}
			}
		}
	}


    void recognizeOfflineData() {
         map< string,  map<int,  map< string, int>>> score;
        for (auto& user : preProcessedData) {
            score[user.first] =  map< string, double>();
            for (int gesture = 1; gesture <= 10; gesture++) {
                score[user.first][gesture] = 0.0;
                for (int i = 1; i <= 100; i++) {
                    auto [training_set, testing_set] = getSplitData(preProcessedData[user.first]["medium"], 9);
					GestureRecognizer recognizer(training_set);
                    auto& templates = training_set[gesture];
                    auto& candidates = preProcessedData[user.first]["medium"][gesture];
                     vector<int> selected_templates;
                     vector<int> selected_candidates;
                    // select E random templates
                    while (selected_templates.size() < 9) {
                        int idx = rand() % templates.size();
                        if ( find(selected_templates.begin(), selected_templates.end(), idx) == selected_templates.end()) {
                            selected_templates.push_back(idx);
                        }
                    }
                    // select one random candidate
                    int candidate_idx = rand() % candidates.size();
                     string candidate_name = candidates[candidate_idx].first;
                    // recognize candidate using selected templates
                    auto [recognizedGesture, _, _, Nbest] = recognizer.recognizeGesture(candidates[candidate_idx].second, templates, 1);
                    recognizedGesture = recognizedGesture.find("/") !=  string::npos ? recognizedGesture.substr(0, recognizedGesture.find("/")) : "";
                    if (recognizedGesture == candidate_name) {
                        score[user.first][gesture] += 1.0;
                    }
                }
                score[user.first][gesture] /= 100.0;
            }
        }

        writeToFile(dumps(score), "score.json");
        // calculate and output average accuracy
        double total_score = 0.0;
        int total_count = 0;
        for (auto& user : score) {
            for (auto& example : user.second) {
                for (auto& gesture : example.second) {
                    total_score += gesture.second;
                    total_count++;
                }
            }
        }
        double average_accuracy = total_score / total_count;
         cout << "Average accuracy: " << average_accuracy <<  endl;
    }

	 pair< map< string,  vector< vector<Point>>>,  map< string,  vector<Point>>> getSplitData( map< string,  vector< vector<Point>>> gestures, int E) {
		 map< string,  vector< vector<Point>>> training_set;
		 map< string,  vector<Point>> testing_set;
		 srand( time(nullptr));
		for (auto& gesture : gestures) {
			for (int i = 0; i < E; i++) {
				training_set[gesture.first + "/" +  to_string(i)] = gesture.second[i];
			}
			testing_set[gesture.first] = gesture.second[ rand() % (10 - E) + E];
		}
		return  make_pair(training_set, testing_set);
	}
// TODO    writetofile funciton