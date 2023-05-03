let sz = 8;
let step = 1;

let dataStructured = [1, 30, 33, 16, 3, 24, 32, 17, 2, 23, 34, 15, 29, 36, 31, 14, 25, 4, 18, 9, 6, 35, 22, 13, 7, 28, 11, 20, 5, 26, 10, 19, 8, 27, 12, 21];

var table = document.createElement("div");
table.id = "chessboard";
document.body.appendChild(table);
renderBoard();

function setSize() {
	x = document.getElementById("inputSize").value;
	n = parseInt(x);
	if (!isNaN(n) && n >= 0) {
		sz = n;
		renderBoard();
	}
}

function renderBoard() {
	var rmv = document.getElementById("chessboard");
	rmv.remove();
	var table = document.createElement("div");
	table.id = "chessboard";
	for (var i = 0; i < sz; i++) {
		var tr = document.createElement('div');
		tr.className = "row";
		for (var j = 0; j < sz; j++) {
			var tcell = document.createElement('span');
			if (i % 2 == j % 2) {
				tcell.className = "cell white";
			} else {
				tcell.className = "cell black";
			}
			if(sz%2 == 0 && j==sz/2-1)	tcell.style.borderRight = "solid green";
			if(sz%2 == 0 && i==sz/2-1) {
				tcell.style.borderBottom = "solid green";
			}
			tcell.innerHTML = "&nbsp";
			tcell.addEventListener("click", fillNumber);
			tr.appendChild(tcell);
		}
		table.appendChild(tr);
	}
	document.body.appendChild(table);
	step = 1;
}

function fillNumber(evt) {
	let x = parseInt(evt.currentTarget.innerHTML);
	if(isNaN(x)) {
		evt.currentTarget.innerHTML = step;
		step++;
	} else {
		x = evt.currentTarget.innerHTML;
		if(x == step-1) {
			evt.currentTarget.innerHTML = "&nbsp";
		} else {
			for(let elem of document.getElementsByClassName("cell")) {
				if(parseInt(elem.innerHTML) >= x) {
					elem.innerHTML = "&nbsp";
				}
			}
		}
		// this solves the bug: after repeatedly pressing copy, it should erase element and set step correctly.
		step = 0;
		for(let elem of document.getElementsByClassName("cell")) {
			let y = parseInt(elem.innerHTML);
			if(!isNaN(y)) {
				step = Math.max(step, y);
			}
		}
		step++;
	}
}

function fillStructured() {
	if(sz % 6 == 0) {
		rowsElem = document.getElementsByClassName("row");
		for(let i = 0; i < 6; ++i) {
			cellsElem = rowsElem[i].getElementsByClassName("cell");
			for(let j = 0; j < 6; ++j) {
				cellsElem[j].innerHTML = dataStructured[6*i+j];
			}
		}
		step = 37;
	}
	console.log(step);
}

function copyTo(a, b) {
	rowsElem = document.getElementsByClassName("row");

	let addend = 0;	
	if(a == 1 || a == 2) {
		for(let i = 0; i < sz/2; ++i) {
			cellsElem = rowsElem[i].getElementsByClassName("cell");
			if(a == 1)
				for(let j = 0; j < sz/2; ++j) {
					let x = parseInt(document.getElementsByClassName("row")[i].getElementsByClassName("cell")[j].innerHTML);
					if(!isNaN(x)) {
						addend = Math.max(step-x, addend);
					}
				}
			if(a == 2)
				for(let j = sz/2; j < sz; ++j) {
					let x = parseInt(document.getElementsByClassName("row")[i].getElementsByClassName("cell")[j].innerHTML);
					if(!isNaN(x)) {
						addend = Math.max(step-x, addend);
					}
				}
		}
	}
	if(a == 3 || a == 4) {
		for(let i = sz/2; i < sz; ++i) {
			cellsElem = rowsElem[i].getElementsByClassName("cell");
			if(a == 3)
				for(let j = 0; j < sz/2; ++j) {
					let x = parseInt(document.getElementsByClassName("row")[i].getElementsByClassName("cell")[j].innerHTML);
					if(!isNaN(x)) {
						addend = Math.max(step-x, addend);
					}
				}
			if(a == 4)
				for(let j = sz/2; j < sz; ++j) {
					let x = parseInt(document.getElementsByClassName("row")[i].getElementsByClassName("cell")[j].innerHTML);
					if(!isNaN(x)) {
						addend = Math.max(step-x, addend);
					}
				}
		}
	}

	if(b == 1 || b == 2) {
		for(let i = 0; i < sz/2; ++i) {
			cellsElem = rowsElem[i].getElementsByClassName("cell");
			if(b == 1) {
				for(let j = 0; j < sz/2; ++j) {
					let x = parseInt(document.getElementsByClassName("row")[i + (a == 3 || a == 4 ? sz/2 : 0)].getElementsByClassName("cell")[j + (a == 2 || a == 4 ? sz/2 : 0)].innerHTML);
					if(!isNaN(x)) {
						cellsElem[j].innerHTML = x + addend;
						step = Math.max(step, x + addend + 1);
					}
				}
			}
			if(b == 2) {
				for(let j = sz/2; j < sz; ++j) {
					let x = parseInt(document.getElementsByClassName("row")[i + (a == 3 || a == 4 ? sz/2 : 0)].getElementsByClassName("cell")[j - (a == 1 || a == 3 ? sz/2 : 0)].innerHTML);
					if(!isNaN(x)) {
						cellsElem[j].innerHTML = x + addend;
						step = Math.max(step, x + addend + 1);
					}
				}
			}
		}
	}
	if(b == 3 || b == 4) {
		for(let i = sz/2; i < sz; ++i) {
			cellsElem = rowsElem[i].getElementsByClassName("cell");
			if(b == 3) {
				for(let j = 0; j < sz/2; ++j) {
					let x = parseInt(document.getElementsByClassName("row")[i - (a == 1 || a == 2 ? sz/2 : 0)].getElementsByClassName("cell")[j + (a == 2 || a == 4 ? sz/2 : 0)].innerHTML);
					if(!isNaN(x)) {
						cellsElem[j].innerHTML = x + addend;
						step = Math.max(step, x + addend + 1);
					}
				}
			}
			if(b == 4) {
				for(let j = sz/2; j < sz; ++j) {
					let x = parseInt(document.getElementsByClassName("row")[i - (a == 1 || a == 2 ? sz/2 : 0)].getElementsByClassName("cell")[j - (a == 1 || a == 3 ? sz/2 : 0)].innerHTML);
					if(!isNaN(x)) {
						cellsElem[j].innerHTML = x + addend;
						step = Math.max(step, x + addend + 1);
					}
				}
			}
		}
	}
}
