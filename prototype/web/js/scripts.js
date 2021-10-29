let radio_clicked = false;

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
  $("#p2p__").css("visibility", "visible");
});

$("#gan_dataset").click(function(){
  $("#p2p__").css("visibility", "hidden");

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

  alert("if already paired (side by side) then gos straight to training on build");
});
