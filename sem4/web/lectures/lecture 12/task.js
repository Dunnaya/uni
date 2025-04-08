// a
function fizzbuzz(n) {
    for(let i = 1; i <=n; i++) {
        if (i % 3 === 0) console.log("fizz");
        else if (i % 5 === 0) console.log("buzz");
        else if(i % 3 === 0 && i % 5 === 0) console.log("fizzbuzz");
        else console.log(i);
    }
}
// b
function cost(uah, kop) {
    let totalKop = uah * 100 + kop;
    let finalUah = Math.floor(totalKop / 100);
    let finalKop = totalKop % 100;

    let uahWord;
    if (finalUah % 10 === 1 && finalUah % 100 !== 11) uahWord = "гривня";
    else if (finalUah === 2 || finalUah === 3 || finalUah === 4) uahWord = "гривні";
    else uahWord = "гривень";
  
    let kopWord;
    if (finalKop % 10 === 1 && finalKop % 100 !== 11) kopWord = "копійка";
    else if (finalKop === 2 || finalKop === 3 || finalKop === 4) kopWord = "копійки";
    else kopWord = "копійок";

    console.log(`Вартість замовлення: ${finalUah} ${uahWord} ${finalKop} ${kopWord}`);
}
// c
function pointInfo(x1, y1, x2, y2) {
    function getQuarter(x, y) {
        if (x > 0 && y > 0) return "в 1 чверті";
        if (x < 0 && y > 0) return "в 2 чверті";
        if (x < 0 && y < 0) return "в 3 чверті";
        if (x > 0 && y < 0) return "в 4 чверті";
        if (x === 0 && y === 0) return "в центрі осі";
        if (x === 0) return "на осі Y";
        if (y === 0) return "на осі X";
      }

    let q1 = getQuarter(x1, y1);
    let q2 = getQuarter(x2, y2);

    if (q1 === q2 && q1 !== "в центрі осі") {
        let distance = Math.sqrt(Math.pow(x2 - x1, 2) + Math.pow(y2 - y1, 2));
        console.log("Відстань між точками (" + x1 + ";" + y1 + ") та (" + x2 + ";" + y2 + "): " + distance.toFixed(2));
    } 
    else {
        console.log("Точка 1 (" + x1 + ";" + y1 + "): " + q1);
        console.log("Точка 2 (" + x2 + ";" + y2 + "): " + q2);
    }
}

fizzbuzz(5);
cost(2, 75);
cost(0, 1);
cost(25, 3);
cost(20, 102);
pointInfo(1, 2, 3, 4);
pointInfo(0, 0, 0, 1);
pointInfo(1, 2, -3, -4);