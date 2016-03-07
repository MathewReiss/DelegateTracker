Pebble.addEventListener("ready", function(e){
	var xhr = new XMLHttpRequest();
	xhr.open("GET", "http://www.politico.com/2016-election/results/delegate-count-tracker", false);
	xhr.send();
	
	var html = xhr.response;
	
	var dem_table = html.substring(html.indexOf("<table class=\"results-table\">"),html.indexOf("</table>")+8).replace(/<[^>]*>/g, "").replace(/^\s+|\s+$|\s+(?=\s)/g, "").replace(",","").split(' ');
	var rep_table = html.substring(html.lastIndexOf("<table class=\"results-table\">"),html.lastIndexOf("</table>")+8).replace(/<[^>]*>/g, "").replace(/^\s+|\s+$|\s+(?=\s)/g, "").replace(",","").split(' ');
	
	var dem_temp = "", rep_temp = "";
	
	for(var d = 0; d < dem_table.length; d++){
		if(isNaN(dem_table[d])) dem_temp += dem_table[d];
		else{
			dem_temp = dem_temp.replace(".", ". ");
			localStorage.setItem(dem_temp,dem_table[d]);
			dem_temp = "";
		}
	}
	
	for(var r = 0; r < rep_table.length; r++){
		if(isNaN(rep_table[r])) rep_temp += rep_table[r];
		else{
			rep_temp = rep_temp.replace(".", ". ");
			localStorage.setItem(rep_temp,rep_table[r]);
			rep_temp = "";
		}
	}
	
	Pebble.sendAppMessage({
		"HILLARY" : localStorage.getItem("H. Clinton"),
		"BERNIE" : localStorage.getItem("B. Sanders"),
		"TRUMP" : localStorage.getItem("D. Trump"),
		"CRUZ" : localStorage.getItem("T. Cruz"),
		"RUBIO" : localStorage.getItem("M. Rubio"),
		"KASICH" : localStorage.getItem("J. Kasich"),
		"UNCOMMITTED_R" : localStorage.getItem("Uncommitted"),
		"UNCOMMITTED_D" : localStorage.getItem("Uncommitted")
	});
});