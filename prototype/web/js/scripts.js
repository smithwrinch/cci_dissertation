let radio_clicked = false;

let p2p = true;
let training = false;

$(".scroll_").click(function(){
  $(".scroll_").css("border", "None");
  $(this).css("border", "2px solid white");
  $(this).css("border-right", "0px solid white");

  $(".scrollHide").css("visibility", "visible");

});

$("#delete").click(function(){
  alert("Deletes selected");
});

$("#loadModel").click(function(){
  alert("It would load into the models last state (ie not always Architecture)");
});



$("#p2p").click(function(){
  $("#architecture").attr("src", "img/architecture.png");
});

$("#gan").click(function(){
    $("#architecture").attr("src", "img/architecture_gan.png");
});



$("#p2p_dataset").click(function(){
  $(".p2p__").css("visibility", "visible");
  p2p = true;

});

$("#gan_dataset").click(function(){
  $(".p2p__").css("visibility", "hidden");
  p2p = false;


});

$("#loadFromFolder").click(function(){
  alert("opens file explorer to select folder full of images");
});

$("#p2p__").click(function(){
  radio_clicked = !radio_clicked;
  if(radio_clicked){
    $("#alreadyPaired").attr("src", "img/icon-radio-off.png");
  }
  else{
    $("#alreadyPaired").attr("src", "img/icon-radio-on.png");
  }

  alert("if already paired (side by side) then goes straight to training on build");
});


$("#startTraining").click(function(){
  $("#lol").text("RESUME TRAINING");
    $(".training").css("visibility", "visible");
    $(".notTraining").css("visibility", "hidden");
    $("#trainButton").text("-TRAINING-");
    training = true;

});


$("#stopTraining").click(function(){
  $(".training").css("visibility", "hidden");
    $(".notTraining").css("visibility", "visible");
    $("#trainButton").text("-TRAIN-");
    training = false;
});


$("#saveModelEpoch").click(function(){
  alert("Opens file explorer and saves model at specified path");
});


$("#p2p_train").click(function(){
  if(training){
    $(".p2p").css("visibility", "visible");
    $(".gan").css("visibility", "hidden");
  }
});

$("#gan_train").click(function(){
  if(training){
  $(".p2p").css("visibility", "hidden");
  $(".gan").css("visibility", "visible");
}
});
$("#cv").click(function(){
  alert("Won't be scaled this small in software");
});

$("#tar").click(function(){
  alert("Opens file explorer to select corresponding image");
});

$("#halffinished").click(function(){
  alert("opens a half finished dataset");
});
