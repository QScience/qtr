function PopByQuality(){
    document.getElementById('quality').className='Visible';
    document.getElementById('download').className='listsHidden';
    document.getElementById('byquality').className='optionlink optionlinkon';
    document.getElementById('bydownload').className='optionlink optionlinkoff';
}
function PopByBownload(){
    document.getElementById('quality').className='listsHidden';
    document.getElementById('download').className='Visible';
    document.getElementById('byquality').className='optionlink optionlinkoff';
    document.getElementById('bydownload').className='optionlink optionlinkon';
}