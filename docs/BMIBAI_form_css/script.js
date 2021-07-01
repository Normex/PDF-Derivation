all_fields.push({name:"text_firstname", id:"20", annots:[]});
all_fields.push({name:"text_lastname", id:"21", annots:[]});
all_fields.push({name:"text_height", id:"22", annots:[]});
function K22() {
AFNumber_Keystroke(1, 0, 0, 0, "", true);
}
function F22() {
AFNumber_Format(1, 0, 0, 0, "", true);
}
all_fields.push({name:"dropdown_height", id:"23", annots:[]});
function V23() {
var height = this.getField("text_height").value;
if(height > 0){
  if(event.value=="cm")
    this.getField("text_height").value = height*2.54;
  else
    this.getField("text_height").value = height/2.54;
}

}
all_fields.push({name:"text_weight", id:"24", annots:[]});
function K24() {
AFNumber_Keystroke(1, 0, 0, 0, "", true);
}
function F24() {
AFNumber_Format(1, 0, 0, 0, "", true);
}
all_fields.push({name:"dropdown_weight", id:"25", annots:[]});
function V25() {
var weight = this.getField("text_weight").value;
if(weight > 0){
  if(event.value=="kg")
    this.getField("text_weight").value = weight/2.205;
  else
    this.getField("text_weight").value = weight*2.205;
}

}
all_fields.push({name:"group_gender", id:"28", annots:[]});
all_fields.push({name:"group_gender", id:"26", annots:[]});
all_fields.push({name:"listbox_ethnic", id:"27", annots:[]});
all_fields.push({name:"text_age", id:"40", annots:[]});
function K40() {
AFNumber_Keystroke(0, 0, 0, 0, "", true);
}
function F40() {
AFNumber_Format(0, 0, 0, 0, "", true);
}
function Bl40() {

if(this.getField("text_age").value >= 18){
  this.getField("checkbox_adult").checkThisBox(0,true);
}
else{
  this.getField("checkbox_adult").checkThisBox(0,false);
}

}
all_fields.push({name:"checkbox_adult", id:"29", annots:[]});
all_fields.push({name:"text_hip", id:"30", annots:[]});
function K30() {
AFNumber_Keystroke(1, 0, 0, 0, "", true);
}
function F30() {
AFNumber_Format(1, 0, 0, 0, "", true);
}
all_fields.push({name:"dropdown_hip", id:"31", annots:[]});
function V31() {
var hip = this.getField("text_hip").value;
if(hip > 0){
  if(event.value=="cm")
    this.getField("text_hip").value = hip*2.54;
  else
    this.getField("text_hip").value = hip/2.54;
}

}
all_fields.push({name:"text_bmi", id:"32", annots:[]});
function K32() {
AFNumber_Keystroke(2, 0, 0, 0, "", true);
}
function F32() {
AFNumber_Format(2, 0, 0, 0, "", true);
}
all_fields.push({name:"text_bmiresult", id:"33", annots:[]});
all_fields.push({name:"text_bai", id:"34", annots:[]});
function K34() {
AFNumber_Keystroke(2, 0, 0, 0, "", true);
}
function F34() {
AFNumber_Format(2, 0, 0, 0, "", true);
}
all_fields.push({name:"text_bairesult", id:"35", annots:[]});
all_fields.push({name:"button_reset", id:"36", annots:[]});
function U36() {

var bmires_val = this.getField("text_bmiresult");
var baires_val = this.getField("text_bairesult");
var submitbtn = this.getField("button_submit");
bmires_val.hidden = true;
baires_val.hidden = true;
submitbtn.hidden = true;
}
all_fields.push({name:"button_calculate", id:"37", annots:[]});
function U37() {

var africans = [ "22", "28", "33", "38", "43" ];
var europeans = [ "19", "25", "30", "35", "40" ];
var indians = [ "16", "22", "27", "32", "37" ];
var asians = [ "18.5", "24", "29", "34", "39" ];
var femalConst = 1.3;

var bai_male = ["8", "19", "25"];
var bai_female = ["21", "33", "39"];

var bmiresA = ["Underweight", "Normal weight", "Overweight", "Obesity level I", "Obesity level II", "Obesity level III"];

var height_val = this.getField("text_height").value;
var height_unit = this.getField("dropdown_height").value;
var weight_val = this.getField("text_weight").value;
var weight_unit = this.getField("dropdown_weight").value;
var hip_unit = this.getField("dropdown_hip").value;
var hip_val = this.getField("text_hip").value;
var bmi = this.getField("text_bmi");
var bai = this.getField("text_bai");

if(height_val>0 && weight_val>0){
  if(height_unit == "inch")
    height_val = height_val*2.54;
  if(weight_unit == "pound")
    weight_val=weight_val/2.205;
  bmi.value=weight_val/(Math.pow(height_val/100,2));
  if(hip_val>0) {
    if(hip_unit == "inch")
      hip_val = hip_val*2.54;
    bai.value=(hip_val/(Math.pow(height_val/100,1.5)))-18;
  }
  else {
    bai.value="";
  }
}
else{
  bmi.value="";
}


var gender = this.getField("group_gender").value;
var ethnic = this.getField("listbox_ethnic").value;
var bmires_val = this.getField("text_bmiresult");
var bmires = "";
var baires_val = this.getField("text_bairesult");
var baires = "";
var gender = this.getField("group_gender").value;
var submitBtn = this.getField("button_submit");

if(bmi.value > 0){
  if(ethnic == "Asians"){
    bmires = setRes(bmi.value, asians, gender);
  }
  else if(ethnic == "Africans"){
    bmires = setRes(bmi.value, africans, gender);
  }
  else if(ethnic == "Europeans"){
    bmires = setRes(bmi.value, europeans, gender);
  }
  else if(ethnic == "Indians"){
    bmires = setRes(bmi.value, indians, gender);
  }
  bmires_val.hidden = false;
  bmires_val.value = bmires;
  submitBtn.hidden = false;
}
else {
  bmires_val.hidden = true;
  submitBtn.hidden = true;
  bmires_val.value = ""; 
  app.alert("Please fill out the height and the weight to calculate the BMI.");
}

if(bai.value > 0){
  if(gender == "female"){
    baires = setRes(bai.value, bai_female, "");
  }
  else {
    baires = setRes(bai.value, bai_male, "");
  }
  baires_val.hidden = false;
  baires_val.value = baires;
}
else {
  baires_val.hidden = true;
  baires_val.value = ""; 
  app.alert("Please fill out the height and the hip to calculate the BAI.");
}

function setRes(bmi_val, param, gender) {
  var bmires = "";
  var val = 0;
  for(i=0; i<param.length; i++) {
    if(gender !="" && gender == "female")
      val = param[i] - femalConst;
    else
      val = param[i];
    if(bmi_val < val){
      bmires = bmiresA[i];
      break;
    }
    else{
      bmires = bmiresA[i+1];
    }
  }
  return bmires;
}
ýýýýÝÝÝÝÝŒB‘ÛB…
}
all_fields.push({name:"button_submit", id:"38", annots:[]});
