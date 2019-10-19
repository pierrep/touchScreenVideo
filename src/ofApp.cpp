#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(0, 0, 0);
	ofSetVerticalSync(true);
    ofSetLogLevel(OF_LOG_NOTICE);
    ofHideCursor();
    
    
	if(touch.init("/dev/input/by-id/usb-ILITEK_Multi-Touch-V300__V300_-event-if01")) {		
		ofLogNotice() << "Using touchscreen input ";
		bUseMouse = false;
	}
	else {
		ofLogNotice() << "Using mouse input";
		bUseMouse = true;
	}

    setupVideos();
    setupGui();
    setupIcons();
    setupFonts();
    
	vidsettings.useHDMIForAudio = true;	//default true
	vidsettings.enableLooping = false;		//default true
	vidsettings.enableTexture = true;		//default true
	//vidsettings.listener = this;			//this app extends ofxOMXPlayerListener so it will receive events ;
	video_player.setup(vidsettings); 

    // set up video sizes
    video_width = ofGetWidth();
    video_height = ofGetWidth() / 1.777776f;
    video_pos_y = (ofGetHeight() / 2) - (video_height / 2);

	// stats and controls
	show_stats = false;
	show_controls = false;

	// timers
	controlbar_start_time = ofGetElapsedTimeMillis();
    fade_in_timer = ofGetElapsedTimeMillis();

	// menu
	show_menu = true;

	num_video_items = video_items.size();
	updateMenuItems();

}

// Launch Video (called from menu)
//--------------------------------------------------------------
void ofApp::launchVideo(unsigned int videoId) {
	ofLogNotice("\n***** Loading video: ") << video_items[videoId].videoFile;
		
	video_player.loadMovie(ofToDataPath(video_items[videoId].videoFile));

	controlbar_start_time = ofGetElapsedTimeMillis();
	show_controls = true;
	paused = false;
	show_menu = false;
}

// Update Menu Items (called during setup, and whenever the window is resized
//--------------------------------------------------------------

void ofApp::updateMenuItems() {
	ofLogNotice("updating menu items - reloading videos!");
    menu_background.load(static_cast<string>(menu_background_filename));
    menu_background.resize(ofGetHeight() * 1.777777778f, ofGetHeight());

    // update video launch areas
    bool bLoaded = settings.load("settings.xml");
    if(bLoaded) {
        settings.pushTag("touchscreen");
        unsigned int numTags = settings.getNumTags("itembox");
        for(unsigned int i = 0; i < video_items.size();i++)
        {
            if(i > (numTags - 1)) break;
            video_items[i].itemBox.x = settings.getValue("itembox:x", 0, i);
            video_items[i].itemBox.y = settings.getValue("itembox:y", 0, i);
            video_items[i].itemBox.width = settings.getValue("itembox:width", 0, i);
            video_items[i].itemBox.height = settings.getValue("itembox:height", 0, i);
        }
        settings.popTag();
    }
}

//--------------------------------------------------------------
void ofApp::setVideoPlaypause() {
	controlbar_start_time = ofGetElapsedTimeMillis();
	//paused = !paused;
	//if (paused) 
	{
		show_controls = true;
	}
//	video_player.setPaused(paused);

}

//--------------------------------------------------------------
void ofApp::returnToMenu() {
	ofLogNotice() << "Return to menu";
	show_menu = true;
	video_player.close();
    fade_in_timer = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
void ofApp::update(){	
	if((video_player.isPlaying()) && (video_player.getCurrentFrame() > 0)) {
		if ((video_player.getMediaTime()/video_player.getDurationInSeconds()) > 0.9999f && show_menu == false) {
			returnToMenu();
		}	
	}
	
	if(bUseMouse) {
		if(bMouseReleased) {
			cout << "mousex: " << mousex << " mousey: " << mousey << endl;
			handleVideoTouch(mousex, mousey,0);
			bMouseReleased = false;
		}
	}
	else {
		handleVideoTouch(touch.getCoordTouch().x, touch.getCoordTouch().y,touch.getButton());
	}
	
	if(touch.getButton() == 1) show_controls = true;

}

//--------------------------------------------------------------
void ofApp::drawVideo() {
	cout << "video_player.getCurrentFrame() = " << video_player.getCurrentFrame() << endl;

	ofSetColor(255, 255, 255);
	if(video_player.getCurrentFrame() > 0) {
		video_player.draw(0, video_pos_y, video_width, video_height);
		video_player.setVolumeNormalized(1.0f);
	}
	

	float controlbar_timer = ofGetElapsedTimeMillis() - controlbar_start_time;

	float hide_anim_timer = ofMap(controlbar_timer, controlbar_show_length, controlbar_show_length + controlbar_anim_length, 1.0, 0.0);
	float hide_back_anim_timer = ofMap(controlbar_timer, controlbar_show_length, controlbar_show_length + controlbar_anim_length, 0.0, 1.0);

	if (controlbar_timer <= controlbar_show_length) {
		controlbar_pos_y = ofGetHeight() - controlbar_height;
		icon_back_pos_x = 30;
	}
	
	
	if ((controlbar_timer >= controlbar_show_length) &&
		!paused &&
		(controlbar_timer <= controlbar_show_length + controlbar_anim_length + 1)) {
		// calculate position of the controlbar
		controlbar_pos_y = ofGetHeight() - (controlbar_height * hide_anim_timer);
		// calculate position of the back button
		icon_back_pos_x = 30 - ((icon_padding + icon_size + 40) * hide_back_anim_timer);
		show_controls = false;
	}
	
	//ofEnableAlphaBlending();
/*
	// Draw control bar
	ofSetColor(26, 26, 26, 96);
	ofEnableAlphaBlending();
	ofDrawRectangle(0, controlbar_pos_y, controlbar_width, controlbar_height);

	// Draw progress bar
	//ofSetHexColor(0x333333);
	ofSetColor(0, 0, 0, 96);
	progress_bar.x = 50;
	progress_bar.y = controlbar_pos_y + 15;
	progress_bar.width = controlbar_width - 100;
	progress_bar.height = icon_size / 2;
	ofDrawRectRounded(progress_bar, 15);
*/
	
	//ofSetHexColor(0xFC4513);
	/*ofSetColor(icon_highlight_color);
	progress_bar_played.x = progress_bar.x;
	progress_bar_played.y = progress_bar.y;
	progress_bar_played.width = progress_bar.width * video_player.getPosition();
	progress_bar_played.height = progress_bar.height;
	ofDrawRectRounded(progress_bar_played, 15);
	*/

	// get position of play/pause button
	//icon_playpause_pos_x = controlbar_width / 2 - icon_size / 2;
	//icon_playpause_pos_y = controlbar_pos_y + (controlbar_height / 2) - (progress_bar.height / 2);

	// draw button background if hovered
	/*if (icon_playpause_hover) {
		ofSetColor(icon_highlight_color);
		icon_playpause_background.x = icon_playpause_pos_x - icon_padding;
		icon_playpause_background.y = icon_playpause_pos_y - icon_padding;
		icon_playpause_background.width = icon_size + (icon_padding * 2);
		icon_playpause_background.height = icon_size + (icon_padding * 2);
		ofDrawRectRounded(icon_playpause_background, 5);
	}
	* */
	// draw background for back button
	ofSetColor(icon_highlight_color);
	icon_back_background.x = icon_back_pos_x - icon_padding;
	icon_back_background.y = 30 - icon_padding;
	icon_back_background.width = icon_size + (icon_padding * 2);
	icon_back_background.height = icon_size + (icon_padding * 2);
	ofDrawRectRounded(icon_back_background, 5);

	// draw button
	ofSetColor(255);
	/*
	if (paused) {
        icon_play.draw(icon_playpause_pos_x, icon_playpause_pos_y);
	}
	else {
		icon_pause.draw(icon_playpause_pos_x, icon_playpause_pos_y);
	}
	*/
	icon_back.draw(icon_back_pos_x, 30);
	//ofDisableAlphaBlending();

}

void ofApp::drawMenu() {
	ofSetColor(255);
	
    float fade_in = ofMap(ofGetElapsedTimeMillis() - fade_in_timer, 0, 1500.0, 0.0, 1.0,true);

	// draw background
	ofEnableAlphaBlending();
    ofSetColor(255, 255, 255, 255 * fade_in);
	menu_background.draw(0, 0);


}

//--------------------------------------------------------------
void ofApp::draw(){
	
	if (show_menu) {
		drawMenu();
	}
	else {
		drawVideo();
	}

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
	case ' ':
		if (!show_menu) {
			//setVideoPlaypause();
		}
		break;
	case 'f':
		ofToggleFullscreen();
		// update video sizes
		video_width = ofGetWidth();
        video_height = ofGetWidth() / 1.777776f;
		video_pos_y = (ofGetHeight() / 2) - (video_height / 2);

		// update control bar size
		controlbar_width = ofGetWidth();

		// update menu item sizes
		updateMenuItems();
		break;
	case 's':
		show_stats = !show_stats;
		break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	ofShowCursor();
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	if(button == 1) std::exit(1);
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	//handleVideoTouch(x,y,button);
	mousex = x;
	mousey = y;
	bMouseReleased = true;
}

//--------------------------------------------------------------
void ofApp::handleVideoTouch(int x, int y, int button)
{
	
	if (!show_menu) { 
		if (x >= icon_back_background.x &&
			x <= icon_back_background.x + icon_back_background.width &&
			y >= icon_back_background.y &&
			y <= icon_back_background.y + icon_back_background.height) {
				ofLogNotice() << "Clicked on return button";
			returnToMenu();
		}		
		else if(x > 0) {
			if((ofGetElapsedTimeMillis() - controlbar_start_time) > 100)
				setVideoPlaypause();
		}
	}
	else if (show_menu) {
        for (size_t i = 0; i < num_video_items; i++) {
			if (x >= video_items[i].itemBox.x &&
				x <= video_items[i].itemBox.x + video_items[i].itemBox.width &&
				y >= video_items[i].itemBox.y &&
				y <= video_items[i].itemBox.y + video_items[i].itemBox.height) {
				launchVideo(i);
			}
		}
	}	
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	// update video sizes
	video_width = ofGetWidth();
    video_height = ofGetWidth() / 1.777776f;
	video_pos_y = (ofGetHeight() / 2) - (video_height / 2);
	
	// update control bar size
	controlbar_width = ofGetWidth();

	// update menu item sizes
	updateMenuItems();
}


//--------------------------------------------------------------
void ofApp::getMetaData(string path, videoItem& vid )
{
    ifstream vidMetaData;

    string fileToOpen = path.replace(path.end() - 4, path.end(), ".txt");

    vidMetaData.open( ofToDataPath(fileToOpen).c_str() );
    ofLog(OF_LOG_VERBOSE) << "metadata file opened: " << vidMetaData.is_open();

    getline(vidMetaData, vid.title);
    ofLog(OF_LOG_VERBOSE) << "video title found: " << vid.title;

    getline(vidMetaData, vid.description);
    ofLog(OF_LOG_VERBOSE) << "video description found: " << vid.description;

    vidMetaData.close();
}

//--------------------------------------------------------------
void ofApp::exit()
{
    gui.saveToFile("gui.xml");
}

//--------------------------------------------------------------
void ofApp::setupGui()
{
    gui.setup("settings");
    gui.add(controlbar_show_length.set("controbar_show_length", 3000.0, 0.0, 10000.0));
    gui.add(controlbar_anim_length.set("controlbar_anim_length", 500.0, 0.0, 10000.0));
    gui.add(icon_padding.set("icon_padding", 10.0, 0.0, 50.0));
    gui.add(fonts_space_between_words.set("fonts_space_between_words", 8.5, 0.0, 50.0));
    gui.add(fonts_title_color.set("font_title_color", ofColor(252, 69, 19), ofColor(0, 0), ofColor(255, 255)));
    gui.add(icon_highlight_color.set("icon_highlight_color", ofColor(252, 69, 19), ofColor(0, 0), ofColor(255, 255)));
    gui.add(menu_background_filename.set("menu_background_img_filename", "backgrounds/background.jpg"));
    gui.add(font_main_normal_filename.set("font_main_regular_filename_ttf", "fonts/opensans-regular.ttf"));
    gui.add(font_main_italic_filename.set("font_main_italic_filename_ttf", "fonts/opensans-italic.ttf"));
    gui.add(font_stats_filename.set("font_stats_filename_ttf", "fonts/opensans-regular.ttf"));

    // Attempt to load XML settings from file
    gui.loadFromFile("gui.xml");
}

//--------------------------------------------------------------
void ofApp::setupVideos()
{
    // read the directory for the images - we know that they are named in seq
    string path = "videos/";
    ofDirectory dir(path);
    dir.allowExt("mp4");

    dir.listDir();
    dir.sort();
    if (!ofDirectory::isDirectoryEmpty(path)) {
        for (size_t i = 0; i < dir.size(); i++) {
            ofLog(OF_LOG_VERBOSE) << "Video found: " << dir.getPath(i);
            videoItem vid;

            vid.videoFile = dir.getPath(i);

            string imageFileTemp = dir.getPath(i);
            vid.imageFile = imageFileTemp.replace(imageFileTemp.end() - 4, imageFileTemp.end(), ".jpg");

            getMetaData(dir.getPath(i),vid);
            video_items.push_back(vid);
        }
    }
    else ofLog(OF_LOG_WARNING) << "Could not find folder";
}

//--------------------------------------------------------------
void ofApp::setupIcons()
{
    // icons
    icon_size = 64;
    icon_size_larger = 80;
    icon_play_select.load("icons/play_select.png");
    icon_play.load("icons/play.png");
    icon_pause.load("icons/pause.png");
    icon_back.load("icons/back.png");
    icon_play_select.resize(icon_size_larger, icon_size_larger);
    icon_play.resize(icon_size, icon_size);
    icon_pause.resize(icon_size, icon_size);
    icon_back.resize(icon_size, icon_size);
    icon_playpause_hover = false;

    controlbar_width = ofGetWidth();
    controlbar_height = icon_size * 2.5f;
    controlbar_pos_y = ofGetHeight();
}

//--------------------------------------------------------------
void ofApp::setupFonts()
{
    ofTrueTypeFont::setGlobalDpi(72);
    font_stats.load(static_cast<string>(font_stats_filename), 14);
    font_stats.setLineHeight(18.0f);
    font_stats.setLetterSpacing(1.037f);

    font_main_normal.load(static_cast<string>(font_main_normal_filename), 24);
    font_main_normal.setLineHeight(30.0f);
    font_main_normal.setLetterSpacing(1);

    font_main_italic.load(static_cast<string>(font_main_italic_filename), 24);
    font_main_italic.setLineHeight(30.0f);
    font_main_italic.setLetterSpacing(1);
}

//--------------------------------------------------------------
