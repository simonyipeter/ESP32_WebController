
$( document ).ready(function() {
    //console.log( "ready!" );
	
	// Instantiate a slider
	$('#csuszka').slider({				});	
	$("#csuszka").on("slide", function(slideEvt) {						$("#slider_val").text(slideEvt.value);					});
		
	$('.row').css('margin-bottom','50px');
	
	setTimeout(function(){
	   window.location.reload(1);
	}, $("#ref_time").val()*1000);
	
	
});





function button_one(){ 
		if($( "#button_one" ).hasClass( "btn-success" )){ 
					$.ajax({  url: "/gpio/5/off",
					   success: function(data){
							if(data=="OK"){	$('#button_one').removeClass("btn-success"); $('#button_one').addClass("btn-danger");	}
					   },  timeout: 1000 //in milliseconds
					});
				}
		else if($( "#button_one" ).hasClass( "btn-danger" )){ 
				
					$.ajax({  url: "/gpio/5/on",
					   success: function(data){
							if(data=="OK"){	$('#button_one').removeClass("btn-danger"); $('#button_one').addClass("btn-success");	}
					   },   timeout: 1000 //in milliseconds
					});
				}
	}
	
function ref_time(){ 
		
					$.ajax({  url: "/ref_time/"+$("#ref_time").val()+"",
					   success: function(data){
							if(data=="OK"){		window.location.reload(1); }
					   },  timeout: 1000 //in milliseconds
					});
				
	}
	
function button_demo_1(){ 
		
					$.ajax({  url: "/demo_1/"+$("#csuszka").val()+"",
					   success: function(data){
							if(data=="OK"){		window.location.reload(1); }
					   },  timeout: 1000 //in milliseconds
					});
				
	}
