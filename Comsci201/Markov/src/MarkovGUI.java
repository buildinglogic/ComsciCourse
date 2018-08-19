/**
 *	GUI for Markov Text Generation Program
 * 
 *	@author Mike Ma
 *	@contributor Brian Lavallee
 */

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.PrintWriter;
import java.net.URL;
import java.util.Observable;
import java.util.Scanner;

import javafx.application.Platform;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.control.Button;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Dialog;
import javafx.scene.control.Label;
import javafx.scene.control.Menu;
import javafx.scene.control.MenuBar;
import javafx.scene.control.MenuItem;
import javafx.scene.control.SeparatorMenuItem;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.control.TextInputDialog;
import javafx.scene.control.TitledPane;
import javafx.scene.control.Tooltip;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.stage.FileChooser;
import javafx.stage.FileChooser.ExtensionFilter;
import javafx.stage.Stage;

public class MarkovGUI {
	private TextGenerator _Model;
	private final VBox _Pane;
	private final Stage _Stage;
	private final TextArea _Text;
	private final TextField _Status;
	private final ControlPane _control;
	private boolean _loaded = false;
	private BufferedInputStream source;
	private Thread worker;

	@SuppressWarnings("static-access")
	public MarkovGUI(Stage stage, TextGenerator model) {
		_Model = model;
		_Pane = new VBox(5);
		_Stage = stage;
		_Stage.setTitle("CompSci 201 Markov");
		_Stage.setScene(new Scene(_Pane));
		_Text = getTextArea();
		_Status = new TextField("Markov");
		_Status.setEditable(false);
		_Stage.setMaxWidth(Double.MAX_VALUE);
		_control = new ControlPane(this);
		_control.addObserver((observable, argument) -> {
			readOld(_control.getK(), _control.getUsing() ? "" : "\\s+");
		});
		readFile(new File("data/alice.txt"), _control.getK(), _control.getUsing() ? "" : "\\s+");
		TitledPane outPane = enclose("output", _Text);
		outPane.setMaxHeight(Double.MAX_VALUE);
		TitledPane msgPane = enclose("message", _Status);
		TitledPane ctlPane = enclose("input", _control.pane());
		_Pane.getChildren().addAll(new Menus(this), ctlPane,outPane, msgPane);
		_Pane.setVgrow(outPane, Priority.ALWAYS);
		stage.show();
	}
	
	public ControlPane getControl() {
		return _control;
	}

	protected Stage getStage() {
		return _Stage;
	}

	private TitledPane enclose(String title, Node n) {
		TitledPane pane = new TitledPane(title, n);
		pane.setCollapsible(false);
		pane.setAlignment(Pos.CENTER);
		return pane;
	}

	private TextArea getTextArea() {
		TextArea txt = new TextArea("CompSci 201 Markov Assignment");
		txt.setEditable(false);
		txt.setWrapText(true);
		txt.setMaxWidth(Double.MAX_VALUE);
		txt.setMaxHeight(Double.MAX_VALUE);
		return txt;
	}

	protected void showErr(Exception e) {
		e.printStackTrace();
	}

	protected void clear() {
		_Text.clear();
		_Status.clear();
	}

	protected void readFile(File file, int k, String delimeter) {
		if (worker != null) {
			try {
				worker.join();
			}
			catch (Exception e) {}
		}
		worker = new Thread(() -> {
			Scanner s = null;
			try {
				long start = System.nanoTime();
				source = new BufferedInputStream(new FileInputStream(file));
				source.mark(Integer.MAX_VALUE);
				s = new Scanner(source);
				int chars = _Model.train(s, delimeter, k);
				long end = System.nanoTime();
				_loaded = true;
				_control.validate();
				String msg = String.format("Read %d keys from %s in %.6f seconds", chars, file.getName(), (end - start) / 1.0e9);
				Platform.runLater(() -> {
					clear();
					showMsg(msg);
				});
				source.reset();
			} catch (Exception e) {
				showErr(e);
			}
		});
		worker.run();
	}

	protected void readURL(String url, int k, String delimeter) {
		if (worker != null) {
			try {
				worker.join();
			}
			catch (Exception e) {}
		}
		worker = new Thread(() -> {
			Scanner s = null;
			try {
				long start = System.nanoTime();
				source = new BufferedInputStream(new URL(url).openStream());
				source.mark(Integer.MAX_VALUE);
				s = new Scanner(source);
				int chars = _Model.train(s, delimeter, k);
				long end = System.nanoTime();
				_loaded = true;
				_control.validate();
				String msg = String.format("Read %d keys from %s in %.6f seconds", chars, url, (end - start) / 1.0e9);
				Platform.runLater(() -> {
					clear();
					showMsg(msg);
				});
				source.reset();
			} catch (Exception e) {
				showErr(e);
			}
		});
		worker.run();
	}
	
	protected void readOld(int k, String delimeter) {
		if (worker != null) {
			try {
				worker.join();
			}
			catch (Exception e) {}
		}
		worker = new Thread(() -> {
			Scanner s = null;
			try {
				long start = System.nanoTime();
				s = new Scanner(source);
				source.mark(Integer.MAX_VALUE);
				int chars = _Model.train(s, delimeter, k);
				long end = System.nanoTime();
				_loaded = true;
				_control.validate();
				String msg = String.format("Read %d keys in %.6f seconds", chars, (end - start) / 1.0e9);
				Platform.runLater(() -> {
					clear();
					showMsg(msg);
				});
				source.reset();
			} catch (Exception e) {
				showErr(e);
			}
		});
		worker.run();
	}
	
	protected boolean getLoaded(){
		return _Model != null && _loaded;
	}

	protected void saveFile(File file) {
		PrintWriter out = null;
		try{
			out = new PrintWriter(new BufferedOutputStream(new FileOutputStream(file)));
			out.print(_Text.getText());
			out.flush();
			out.close();
		}catch(Exception e){
			showErr(e);
			if(out!=null)
				out.close();
		}
	}

	protected void generate(int length) {
		if (worker != null) {
			try {
				worker.join();
			}
			catch (Exception e) {}
		}
		
		try {
			long start = System.nanoTime();
			String s = _Model.generateText(length) + "\n\n";
			long end = System.nanoTime();
			String msg = String.format("Generated %d words in %.3f seconds", s.split("\\s+").length, (end - start) / 1.0e9);
			updateText(s);
			showMsg(msg);
		} catch (Exception e) {
			showErr(e);
		}
	}

	protected void updateText(String s) {
		_Text.appendText(s);
	}

	protected void showMsg(String msg) {
		_Status.setText(msg);
	}
}

class Menus extends MenuBar {
	private MarkovGUI _Gui;

	protected Menus(MarkovGUI gui) {
		_Gui = gui;
		Menu file = new Menu("File");
		Menu help = new Menu("Help");
		MenuItem abt = new MenuItem("About");
		help.getItems().add(abt);
		abt.setOnAction(e->showInfo());
		file.getItems().addAll(getOpen(), getURL(), getSave(), getClear(), new SeparatorMenuItem(), getExit());
		getMenus().addAll(file, help);
	}
	
	private void showInfo(){
		Alert alert = new Alert(AlertType.INFORMATION);
		alert.setTitle("About");
		alert.setContentText("CompSci 201 Markov Assignment\n\nOriginal Author: Owen Astrachan\nGUI developed by Mike Ma");
		alert.showAndWait();
	}

	private MenuItem getOpen() {
		MenuItem item = new MenuItem("Open File");
		item.setAccelerator(new KeyCodeCombination(KeyCode.O, KeyCombination.SHORTCUT_DOWN));
		item.setOnAction(e -> {
			FileChooser fileChooser = new FileChooser();
			fileChooser.setTitle("Open Text File");
			fileChooser.getExtensionFilters().addAll(new ExtensionFilter("Text Files", "*.txt"),
					new ExtensionFilter("All Files", "*.*"));
			File selectedFile = fileChooser.showOpenDialog(_Gui.getStage());
			if (selectedFile != null && selectedFile.canRead()) {
				_Gui.readFile(selectedFile, _Gui.getControl().getK(), _Gui.getControl().getUsing() ? "" : "\\s+");
			}
		});
		return item;
	}

	private MenuItem getURL() {
		MenuItem item = new MenuItem("Open URL");
		item.setAccelerator(new KeyCodeCombination(KeyCode.U, KeyCombination.SHORTCUT_DOWN));
		item.setOnAction(e -> {
			Dialog<String> dialog = new TextInputDialog();
			dialog.setTitle("Enter URL");
			dialog.setHeaderText("Please enter a valid URL");
			dialog.showAndWait().ifPresent(s -> _Gui.readURL(s, _Gui.getControl().getK(), _Gui.getControl().getUsing() ? "" : "\\s+"));
		});
		return item;
	}

	private MenuItem getSave() {
		MenuItem item = new MenuItem("Save Text");
		item.setAccelerator(new KeyCodeCombination(KeyCode.S, KeyCombination.SHORTCUT_DOWN));
		item.setOnAction(e -> {
			FileChooser fileChooser = new FileChooser();
			fileChooser.setTitle("Save Text File");
			fileChooser.getExtensionFilters().add(new ExtensionFilter("Text Files", "*.txt"));
			File selectedFile = fileChooser.showSaveDialog(_Gui.getStage());
			if (selectedFile != null) {
				_Gui.saveFile(selectedFile);
			}
		});
		return item;
	}

	private MenuItem getExit() {
		MenuItem item = new MenuItem("Exit");
		item.setAccelerator(new KeyCodeCombination(KeyCode.Q, KeyCombination.SHORTCUT_DOWN));
		item.setOnAction(e -> {
			System.exit(0);
		});
		return item;
	}

	private MenuItem getClear() {
		MenuItem item = new MenuItem("Clear Text");
		item.setAccelerator(new KeyCodeCombination(KeyCode.C, KeyCombination.SHORTCUT_DOWN));
		item.setOnAction(e -> {
			_Gui.clear();
		});
		return item;
	}
}

class ControlPane extends Observable {
	private TextField _TFK, _TFL;
	private Button _OK;
	private final MarkovGUI _Gui;
	private GridPane pane;
	private boolean usingLetter;

	ControlPane(MarkovGUI gui) {
		_Gui = gui;
		pane = new GridPane();
		pane.setHgap(10);
		pane.setVgap(10);
		pane.setPadding(new Insets(20, 150, 10, 10));
		addButton();
		addK();
		addL();
		addToggle();
		usingLetter = true;
	}
	
	public GridPane pane() {
		return pane;
	}
	
	public int getK() {
		return Integer.parseInt(_TFK.getText().trim());
	}
	
	public boolean getUsing() {
		return usingLetter;
	}
	
	private void addButton(){
		_OK = new Button("Generate");
		pane.add(_OK, 3,0);
		_OK.setOnAction(e->generate());
		_OK.setDisable(true);
	}
	
	private void generate(){
		try {
			int l = Integer.parseInt(_TFL.getText().trim());
			_Gui.generate(l);
		} catch (Exception e) {
			_Gui.showErr(e);
		}
	}

	private void addK() {
		_TFK = new TextField("3");
		_TFK.setPromptText("Enter a positive integer");
		_TFK.setTooltip(new Tooltip("Enter a positive integer"));
		pane.add(new Label("K:"), 0, 0);
		pane.add(_TFK, 1, 0);
		_TFK.textProperty().addListener((ch,oV,nV)-> {
			if (validate()) {
				setChanged();
				notifyObservers();
			}
		});
	}

	private void addL() {
		_TFL = new TextField("100");
		_TFL.setPromptText("Enter a positive integer");
		_TFL.setTooltip(new Tooltip("Enter a positive integer"));
		pane.add(new Label("MaxLength:"), 0, 1);
		pane.add(_TFL, 1, 1);
		_TFL.textProperty().addListener((ch,oV,nV)->validate());
	}
	
	private void addToggle() {
		CheckBox letter = new CheckBox("Letter Model");
		CheckBox word = new CheckBox("Word Model");
		
		letter.setSelected(true);
		word.setSelected(false);
		
		letter.setOnAction((click) -> {
			word.setSelected(!letter.isSelected());
			usingLetter = letter.isSelected();
			setChanged();
			notifyObservers();
		});
		
		word.setOnAction((click) -> {
			letter.setSelected(!word.isSelected());
			usingLetter = letter.isSelected();
			setChanged();
			notifyObservers();
		});
		
		pane.add(letter, 2, 0);
		pane.add(word, 2, 1);
	}

	protected boolean validate() {
		try {
			int k = Integer.parseInt(_TFK.getText().trim());
			int l = Integer.parseInt(_TFL.getText().trim());
			_OK.setDisable(!(k > 0 && l > 0 && _Gui.getLoaded()));
			return true;
		} catch (Exception e) {
			_OK.setDisable(true);
			return false;
		}
	}
}