var refresh_time = 5;

setTimeout(function(){
	   window.location.reload(1);
	}, refresh_time*1000);

$( document ).ready(function() {
    //console.log( "ready!" );
	
	// Instantiate a slider
	$('#csuszka').slider({
						formatter: function(value) { return 'Current value: ' + value;			}
							})	
	$("#csuszka").on("slide", function(slideEvt) {
						refresh_time=slideEvt.value;
						console.log( refresh_time );
					});
		
	$('.row').css('margin-bottom','50px');
	
	
});





function button_one(){ 
		if($( "#button_one" ).hasClass( "btn-success" )){ 
					$.ajax({  url: "/gpio-5/",
					   success: function(data){
							if(data=="OK"){	$('#button_one').removeClass("btn-success"); $('#button_one').addClass("btn-danger");	}
					   },
					   timeout: 1000 //in milliseconds
					});
				}
		else if($( "#button_one" ).hasClass( "btn-danger" )){ 
				
				
					$.ajax({  url: "/gpio-5/",
					   success: function(data){
							if(data=="OK"){	$('#button_one').removeClass("btn-danger"); $('#button_one').addClass("btn-success");	}
					   },
					   timeout: 1000 //in milliseconds
					});
				}
	}